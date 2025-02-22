#include "game/actionperformer.h"
#include "game/gameanimation/gameanimationfactory.h"
#include "ai/coreai.h"

#include "menue/gamemenue.h"
#include "menue/movementplanner.h"

#include "objects/dialogs/dialogconnecting.h"
#include "objects/dialogs/dialogmessagebox.h"

ActionPerformer::ActionPerformer(GameMap* pMap, GameMenue* pMenu)
    : m_pMenu(pMenu),
      m_pMap(pMap)
{
    m_delayedActionPerformedTimer.setSingleShot(true);
    connect(&m_delayedActionPerformedTimer, &QTimer::timeout, this, &ActionPerformer::delayedActionPerformed, Qt::QueuedConnection);
}

bool ActionPerformer::getActionRunning() const
{
    return m_actionRunning;
}

void ActionPerformer::setSyncCounter(qint64 counter)
{
    m_syncCounter = counter;
}

void ActionPerformer::performAction(spGameAction pGameAction)
{
    if (m_actionRunning)
    {
        CONSOLE_PRINT("Ignoring action request cause an action is currently performed", Console::eWARNING);
        return;
    }
    m_actionRunning = true;
    m_pMenu->setSaveAllowed(false);
    if (m_multiplayerSyncData.m_waitingForSyncFinished && m_pMenu != nullptr)
    {
        m_multiplayerSyncData.m_postSyncAction = pGameAction;
        spDialogConnecting pDialogConnecting = spDialogConnecting::create(tr("Waiting for Players/Observers to join..."), 1000 * 60 * 5);
        m_pMenu->addChild(pDialogConnecting);
        connect(pDialogConnecting.get(), &DialogConnecting::sigCancel, m_pMenu, &GameMenue::exitGame, Qt::QueuedConnection);
        connect(m_pMenu, &GameMenue::sigSyncFinished, pDialogConnecting.get(), &DialogConnecting::connected, Qt::QueuedConnection);
    }
    else if (pGameAction.get() != nullptr)
    {
        CONSOLE_PRINT("GameMenue::performAction " + pGameAction->getActionID() + " at X: " + QString::number(pGameAction->getTarget().x())
                      + " at Y: " + QString::number(pGameAction->getTarget().y()), Console::eDEBUG);

        Mainapp::getInstance()->pauseRendering();
        bool multiplayer = false;
        if (m_pMenu != nullptr)
        {
            multiplayer = m_pMenu->getIsMultiplayer(pGameAction);
        }
        Player* pCurrentPlayer = m_pMap->getCurrentPlayer();
        auto* baseGameInput = pCurrentPlayer->getBaseGameInput();
        if (multiplayer &&
            m_pMenu != nullptr &&
            baseGameInput != nullptr &&
            baseGameInput->getAiType() == GameEnums::AiTypes_ProxyAi &&
            m_syncCounter + 1 != pGameAction->getSyncCounter())
        {
            m_pMenu->setGameStarted(false);
            spDialogMessageBox pDialogMessageBox = spDialogMessageBox::create(tr("The game is out of sync and can't be continued. The game has been stopped. You can save the game and restart."));
            m_pMenu->addChild(pDialogMessageBox);
        }
        else
        {
            if (multiplayer &&
                baseGameInput != nullptr &&
                baseGameInput->getAiType() == GameEnums::AiTypes_ProxyAi)
            {
                m_syncCounter = pGameAction->getSyncCounter();
            }
            m_pStoredAction = nullptr;
            m_pMap->getGameRules()->pauseRoundTime();
            if (!pGameAction->getIsLocal() &&
                (baseGameInput != nullptr &&
                 baseGameInput->getAiType() != GameEnums::AiTypes_ProxyAi))
            {
                pGameAction = doMultiTurnMovement(pGameAction);
            }
            Unit * pMoveUnit = pGameAction->getTargetUnit();
            doTrapping(pGameAction);
            // send action to other players if needed
            if (requiresForwarding(pGameAction))
            {
                CONSOLE_PRINT("Sending action to other players", Console::eDEBUG);
                m_syncCounter++;
                pGameAction->setSyncCounter(m_syncCounter);
                pGameAction->setRoundTimerTime(m_pMap->getGameRules()->getRoundTimer()->remainingTime());
                QByteArray data;
                QDataStream stream(&data, QIODevice::WriteOnly);
                stream << m_pMap->getCurrentPlayer()->getPlayerID();
                pGameAction->serializeObject(stream);
                emit m_pMenu->getNetworkInterface()->sig_sendData(0, data, NetworkInterface::NetworkSerives::Game, true);
            }
            else if (multiplayer)
            {
                m_pMap->getGameRules()->getRoundTimer()->setInterval(pGameAction->getRoundTimerTime());
            }

            // record action if required
            if (m_pMenu != nullptr)
            {
                m_pMenu->getReplayRecorder().recordAction(pGameAction);
            }

            // perform action
            GlobalUtils::seed(pGameAction->getSeed());
            GlobalUtils::setUseSeed(true);
            if (pMoveUnit != nullptr)
            {
                pMoveUnit->setMultiTurnPath(pGameAction->getMultiTurnPath());
            }

            if (baseGameInput != nullptr)
            {
                baseGameInput->centerCameraOnAction(pGameAction.get());
            }
            pGameAction->perform();
            // clean up the action
            m_pCurrentAction = pGameAction;
            pGameAction = nullptr;
            skipAnimations(false);
        }
        if (pCurrentPlayer != m_pMap->getCurrentPlayer() &&
            m_pMenu != nullptr)
        {
            auto* baseGameInput = m_pMap->getCurrentPlayer()->getBaseGameInput();
            if (baseGameInput != nullptr &&
                baseGameInput->getAiType() == GameEnums::AiTypes_Human)
            {
                m_pMenu->autoSaveMap();
            }
        }
        Mainapp::getInstance()->continueRendering();
    }
}

bool ActionPerformer::requiresForwarding(const spGameAction & pGameAction) const
{
    Player* pCurrentPlayer = m_pMap->getCurrentPlayer();
    auto* baseGameInput = pCurrentPlayer->getBaseGameInput();
    return m_pMenu != nullptr &&
                      m_pMenu->getIsMultiplayer(pGameAction) &&
                      baseGameInput != nullptr &&
                                       baseGameInput->getAiType() != GameEnums::AiTypes_ProxyAi;
}

spGameAction ActionPerformer::doMultiTurnMovement(spGameAction pGameAction)
{
    if (m_pMenu != nullptr &&
        m_pMenu->getGameStarted() &&
        pGameAction.get() != nullptr &&
        (pGameAction->getActionID() == CoreAI::ACTION_NEXT_PLAYER ||
         pGameAction->getActionID() == CoreAI::ACTION_SWAP_COS))
    {
        CONSOLE_PRINT("Check and update multiTurnMovement", Console::eDEBUG);

        // check for units that have a multi turn available
        qint32 heigth = m_pMap->getMapHeight();
        qint32 width = m_pMap->getMapWidth();
        Player* pPlayer = m_pMap->getCurrentPlayer();
        for (qint32 y = 0; y < heigth; y++)
        {
            for (qint32 x = 0; x < width; x++)
            {
                Unit* pUnit = m_pMap->getTerrain(x, y)->getUnit();
                if (pUnit != nullptr)
                {
                    if ((pUnit->getOwner() == pPlayer) &&
                        (pUnit->getHasMoved() == false))
                    {
                        QVector<QPoint> currentMultiTurnPath = pUnit->getMultiTurnPath();
                        if (currentMultiTurnPath.size() > 0)
                        {
                            qint32 movepoints = pUnit->getMovementpoints(pUnit->getPosition());
                            if (movepoints > 0)
                            {
                                // shorten path
                                UnitPathFindingSystem pfs(m_pMap, pUnit, pPlayer);
                                pfs.setMovepoints(pUnit->getFuel());
                                pfs.explore();
                                auto newPath = pfs.getClosestReachableMovePath(currentMultiTurnPath, movepoints);
                                if (newPath.size() > 0)
                                {
                                    // replace current action with auto moving none moved units
                                    m_pStoredAction = pGameAction;
                                    spGameAction multiTurnMovement = spGameAction::create(CoreAI::ACTION_WAIT, m_pMap);
                                    if (pUnit->getActionList().contains(CoreAI::ACTION_HOELLIUM_WAIT))
                                    {
                                        multiTurnMovement->setActionID(CoreAI::ACTION_HOELLIUM_WAIT);
                                    }
                                    multiTurnMovement->setTarget(pUnit->getPosition());
                                    multiTurnMovement->setMovepath(newPath, pfs.getCosts(newPath));
                                    QVector<QPoint> multiTurnPath;
                                    // still some path ahead?
                                    multiTurnPath = QVector<QPoint>();
                                    if (currentMultiTurnPath.size() > newPath.size())
                                    {
                                        for (qint32 i = 0; i <= currentMultiTurnPath.size() - newPath.size(); i++)
                                        {
                                            multiTurnPath.append(currentMultiTurnPath[i]);
                                        }
                                    }
                                    multiTurnMovement->setMultiTurnPath(multiTurnPath);
                                    return multiTurnMovement;
                                }
                            }
                        }
                        else if (pUnit->getActionList().contains(CoreAI::ACTION_CAPTURE))
                        {
                            spGameAction multiTurnMovement = spGameAction::create(CoreAI::ACTION_CAPTURE, m_pMap);
                            multiTurnMovement->setTarget(pUnit->getPosition());
                            if (multiTurnMovement->canBePerformed())
                            {
                                m_pStoredAction = pGameAction;
                                return multiTurnMovement;
                            }
                        }
                    }
                }
            }
        }
    }
    return pGameAction;
}

void ActionPerformer::centerMapOnAction(GameAction* pGameAction)
{
    if (m_pMenu != nullptr)
    {
        CONSOLE_PRINT("centerMapOnAction()", Console::eDEBUG);
        Unit* pUnit = pGameAction->getTargetUnit();
        Player* pPlayer = m_pMenu->getCurrentViewPlayer();

        QPoint target = pGameAction->getTarget();
        if (pUnit != nullptr &&
            !pUnit->isStealthed(pPlayer))
        {
            const auto & path = pGameAction->getMovePath();
            for (const auto & point : path)
            {
                if (pPlayer->getFieldVisible(point.x(), point.y()))
                {
                    target = point;
                    break;
                }
            }
        }

        if (m_pMap->onMap(target.x(), target.y()) &&
            pPlayer->getFieldVisible(target.x(), target.y()) &&
            (pUnit == nullptr ||
             !pUnit->isStealthed(pPlayer)))
        {
            m_pMap->centerMap(target.x(), target.y());
        }
    }
}

void ActionPerformer::skipAnimations(bool postAnimation)
{
    CONSOLE_PRINT("skipping Animations", Console::eDEBUG);
    Mainapp::getInstance()->pauseRendering();
    if (GameAnimationFactory::getAnimationCount() > 0)
    {
        GameAnimationFactory::skipAllAnimations();
    }
    if (GameAnimationFactory::getAnimationCount() == 0 && !postAnimation)
    {
        CONSOLE_PRINT("GameMenue -> emitting animationsFinished()", Console::eDEBUG);
        emit GameAnimationFactory::getInstance()->animationsFinished();
    }
    Mainapp::getInstance()->continueRendering();
}

void ActionPerformer::finishActionPerformed()
{
    CONSOLE_PRINT("Doing post action update", Console::eDEBUG);

    if (m_pCurrentAction.get() != nullptr)
    {
        Unit* pUnit = m_pCurrentAction->getMovementTarget();
        if (pUnit != nullptr)
        {
            pUnit->postAction(m_pCurrentAction);
        }
        m_pMap->getCurrentPlayer()->postAction(m_pCurrentAction.get());
        m_pMap->getGameScript()->actionDone(m_pCurrentAction);
        m_pCurrentAction = nullptr;
    }
    m_pMap->killDeadUnits();
    m_pMap->getGameRules()->checkVictory();
    skipAnimations(true);
    m_pMap->getGameRules()->createFogVision();
    if (m_pMenu != nullptr)
    {
        m_pMenu->updateQuickButtons();
    }
}

void ActionPerformer::actionPerformed()
{
    if (m_pMenu->getParent() != nullptr &&
        m_pMap != nullptr)
    {
        CONSOLE_PRINT("Action performed", Console::eDEBUG);
        finishActionPerformed();
        if (Settings::getSyncAnimations())
        {
            m_pMap->syncUnitsAndBuildingAnimations();
        }
        if (m_pMenu != nullptr)
        {
            m_pMenu->updateGameInfo();
        }
        if (GameAnimationFactory::getAnimationCount() == 0 &&
            !m_pMap->getGameRules()->getVictory())
        {
            m_actionRunning = false;
            if (!m_pMap->anyPlayerAlive() &&
                m_pMenu != nullptr)
            {
                CONSOLE_PRINT("Forcing exiting the game cause no player is alive", Console::eDEBUG);
                emit m_pMenu->sigExitGame();
            }
            else if (m_pMap->getCurrentPlayer()->getIsDefeated())
            {
                CONSOLE_PRINT("Triggering next player cause current player is defeated", Console::eDEBUG);
                spGameAction pAction = spGameAction::create(CoreAI::ACTION_NEXT_PLAYER, m_pMap);
                performAction(pAction);
            }
            else if (m_pStoredAction.get() != nullptr)
            {
                performAction(m_pStoredAction);
            }
            else
            {
                GlobalUtils::setUseSeed(false);
                if (m_pMap->getCurrentPlayer()->getBaseGameInput()->getAiType() != GameEnums::AiTypes_ProxyAi)
                {
                    m_pMap->getGameRules()->resumeRoundTime();
                }
                if (m_noTimeOut)
                {
                    spGameAction pAction = spGameAction::create(CoreAI::ACTION_NEXT_PLAYER, m_pMap);
                    performAction(pAction);
                    m_noTimeOut = false;
                }
                else
                {
                    CONSOLE_PRINT("emitting sigActionPerformed()", Console::eDEBUG);
                    quint32 delay = Settings::getPauseAfterAction();
                    if (delay == 0)
                    {
                        emit sigActionPerformed();
                    }
                    else
                    {
                        m_delayedActionPerformedTimer.start(std::chrono::seconds(delay));
                    }
                }
            }
        }
    }
    else
    {
        CONSOLE_PRINT("Skipping action performed", Console::eDEBUG);
    }

    if (m_pMenu != nullptr)
    {
        m_pMenu->setSaveAllowed(true);
        if (m_pMenu->getSaveMap())
        {
            m_pMenu->doSaveMap();
        }
    }
}

void ActionPerformer::nextTurnPlayerTimeout()
{
    auto* input = m_pMap->getCurrentPlayer()->getBaseGameInput();
    if (input != nullptr)
    {
        if (input->getAiType() == GameEnums::AiTypes_Human)
        {
            if (m_pCurrentAction.get() == nullptr)
            {
                spGameAction pAction = spGameAction::create(CoreAI::ACTION_NEXT_PLAYER, m_pMap);
                performAction(pAction);
            }
            else
            {
                m_noTimeOut = true;
            }
        }
        else if (input->getAiType() != GameEnums::AiTypes_Human &&
                 input->getAiType() != GameEnums::AiTypes_ProxyAi &&
                 input->getAiType() != GameEnums::AiTypes_Closed)
        {
            m_noTimeOut = true;
        }
    }
}

qint64 ActionPerformer::getSyncCounter() const
{
    return m_syncCounter;
}

void ActionPerformer::doTrapping(spGameAction & pGameAction)
{
    CONSOLE_PRINT("GameMenue::doTrapping", Console::eDEBUG);
    QVector<QPoint> path = pGameAction->getMovePath();

    Unit * pMoveUnit = pGameAction->getTargetUnit();
    if (path.size() > 1 && pMoveUnit != nullptr)
    {
        if (pGameAction->getRequiresEmptyField())
        {
            QVector<QPoint> trapPathNotEmptyTarget = path;
            qint32 trapPathCostNotEmptyTarget = pGameAction->getCosts();
            QPoint trapPoint = path[0];
            for (qint32 i = 0; i < path.size() - 1; i++)
            {
                QPoint point = path[i];
                QPoint prevPoint = path[i + 1];
                Unit* pUnit = m_pMap->getTerrain(point.x(), point.y())->getUnit();
                if (pUnit == nullptr || pMoveUnit->getOwner()->isAlly(pUnit->getOwner()))
                {
                    if (i > 0)
                    {
                        spGameAction pTrapAction = spGameAction::create(CoreAI::ACTION_TRAP, m_pMap);
                        pTrapAction->setMovepath(trapPathNotEmptyTarget, trapPathCostNotEmptyTarget);
                        pTrapAction->writeDataInt32(trapPoint.x());
                        pTrapAction->writeDataInt32(trapPoint.y());
                        pTrapAction->setTarget(pGameAction->getTarget());
                        pGameAction = pTrapAction;
                    }
                    break;
                }
                else
                {
                    trapPoint = point;
                    qint32 moveCost = pMoveUnit->getMovementCosts(point.x(), point.y(),
                                                                  prevPoint.x(), prevPoint.y(), true);
                    trapPathCostNotEmptyTarget -= moveCost;
                    trapPathNotEmptyTarget.removeFirst();
                }
            }
        }
        path = pGameAction->getMovePath();
        QVector<QPoint> trapPath;
        qint32 trapPathCost = 0;
        for (qint32 i = path.size() - 2; i >= 0; i--)
        {
            // check the movepath for a trap
            QPoint point = path[i];
            QPoint prevPoint = path[i];
            if (i > 0)
            {
                prevPoint = path[i - 1];
            }
            qint32 moveCost = pMoveUnit->getMovementCosts(point.x(), point.y(),
                                                          prevPoint.x(), prevPoint.y(), true);
            if (isTrap("isTrap", pGameAction, pMoveUnit, point, prevPoint, moveCost))
            {
                while (trapPath.size() > 1)
                {
                    QPoint currentPoint = trapPath[0];
                    QPoint previousPoint = trapPath[1];
                    moveCost = pMoveUnit->getMovementCosts(currentPoint.x(), currentPoint.y(),
                                                           previousPoint.x(), previousPoint.y());
                    if (isTrap("isStillATrap", pGameAction, pMoveUnit, currentPoint, previousPoint, moveCost))
                    {
                        trapPathCost -= moveCost;
                        trapPath.pop_front();
                        if (m_pMap->getTerrain(point.x(), point.y())->getUnit() != nullptr)
                        {
                            point = currentPoint;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                spGameAction pTrapAction = spGameAction::create(CoreAI::ACTION_TRAP, m_pMap);
                pTrapAction->setMovepath(trapPath, trapPathCost);
                pMoveUnit->getOwner()->addVisionField(point.x(), point.y(), 1, true);
                pTrapAction->writeDataInt32(point.x());
                pTrapAction->writeDataInt32(point.y());
                pTrapAction->setTarget(pGameAction->getTarget());
                pGameAction = pTrapAction;
                break;
            }
            else
            {
                trapPath.push_front(point);
                qint32 x = pMoveUnit->Unit::getX();
                qint32 y = pMoveUnit->Unit::getY();
                if (point.x() != x ||
                    point.y() != y)
                {
                    QPoint previousPoint = path[i + 1];
                    trapPathCost += pMoveUnit->getMovementCosts(point.x(), point.y(), previousPoint.x(), previousPoint.y());
                }
            }
        }
    }
}

bool ActionPerformer::isTrap(const QString & function, spGameAction pAction, Unit* pMoveUnit, QPoint currentPoint, QPoint previousPoint, qint32 moveCost)
{

    Unit* pUnit = m_pMap->getTerrain(currentPoint.x(), currentPoint.y())->getUnit();

    Interpreter* pInterpreter = Interpreter::getInstance();
    QJSValueList args({pInterpreter->newQObject(pAction.get()),
                       pInterpreter->newQObject(pMoveUnit),
                       pInterpreter->newQObject(pUnit),
                       currentPoint.x(),
                       currentPoint.y(),
                       previousPoint.x(),
                       previousPoint.y(),
                       moveCost,
                       pInterpreter->newQObject(m_pMap),
                      });
    QJSValue erg = pInterpreter->doFunction("ACTION_TRAP", function, args);
    if (erg.isBool())
    {
        return erg.toBool();
    }
    return false;
}

void ActionPerformer::delayedActionPerformed()
{
    if (m_pCurrentAction.get() == nullptr)
    {
        emit sigActionPerformed();
    }
}
