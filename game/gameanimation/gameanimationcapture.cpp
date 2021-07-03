#include "game/gameanimation/gameanimationcapture.h"
#include "game/gamemap.h"

#include "resource_management/gameanimationmanager.h"

#include "coreengine/console.h"
#include "coreengine/audiothread.h"
#include "coreengine/mainapp.h"

#include "spritingsupport/spritecreator.h"

const qint32 GameAnimationCapture::m_capturingFactor = 7;
const qint32 GameAnimationCapture::m_jumpSprites = 3;
const qint32 GameAnimationCapture::m_ayeAyeSprites = 2;
const qint32 GameAnimationCapture::m_jumpingCount = 3;

GameAnimationCapture::GameAnimationCapture(qint32 startPoints, qint32 endPoints, qint32 maxPoints)
    : GameAnimation(static_cast<quint32>(GameMap::frameTime)),
      m_startPoints(startPoints),
      m_endPoints(endPoints),
      m_maxPoints(maxPoints)
{
    setObjectName("GameAnimationCapture");
    Mainapp* pApp = Mainapp::getInstance();
    moveToThread(pApp->getWorkerthread());
    Interpreter::setCppOwnerShip(this);
    m_frameTime = GameMap::frameTime / Settings::getCaptureAnimationSpeed();
}

void GameAnimationCapture::addBuildingSprite(QString spriteID, Player* startPlayer, Player* capturedPlayer, GameEnums::Recoloring mode)
{
    Mainapp* pApp = Mainapp::getInstance();
    GameAnimationManager* pGameAnimationManager = GameAnimationManager::getInstance();
    oxygine::ResAnim* pAnim = pGameAnimationManager->getResAnim(spriteID);
    if (mode == GameEnums::Recoloring_Mask)
    {
        oxygine::handleErrorPolicy(oxygine::ep_show_error, "GameAnimationCapture::addBuildingSprite " + spriteID + " recloring mode mask not supported!");
    }
    if (pAnim != nullptr)
    {
        createBuildingActor(pAnim, startPlayer, capturedPlayer);

        if (m_buildingSprites.get() == nullptr)
        {
            m_buildingSprites = oxygine::spSprite::create();
            m_buildingSprites->setDestRecModifier(oxygine::RectF(0.5f, 0.5f, 0.0f, 0.0f));
            m_BuildingActor->addChild(m_buildingSprites);
            m_buildingResAnim = oxygine::spSingleResAnim::create();
            m_captureBuildingResAnim = oxygine::spSingleResAnim::create();
        }
        QImage preCaptureImage(pAnim->getResPath());
        QImage captureImage(pAnim->getResPath());
        getRecoloredImage(startPlayer, capturedPlayer, mode,
                          preCaptureImage, captureImage);
        pApp->loadResAnim(m_buildingResAnim, m_buildingImage, 1, 1, 1.0f, false);
        pApp->loadResAnim(m_captureBuildingResAnim, m_captureBuildingImage, 1, 1, 1.0f, false);
        m_buildingSprites->setResAnim(m_buildingResAnim.get());
    }
}

void GameAnimationCapture::getRecoloredImage(Player* startPlayer, Player* capturedPlayer, GameEnums::Recoloring mode,
                                             QImage & preCaptureImage, QImage & captureImage)
{
    if (mode == GameEnums::Recoloring_Table)
    {
        if (startPlayer == nullptr)
        {
            QImage table = Player::getNeutralTableImage();
            m_buildingImage = SpriteCreator::recolorImageWithTable(preCaptureImage, table);
        }
        else
        {
            QImage table = startPlayer->getColorTable();
            m_buildingImage = SpriteCreator::recolorImageWithTable(preCaptureImage, table);
        }
        if (capturedPlayer != nullptr)
        {
            QImage table = capturedPlayer->getColorTable();
            m_captureBuildingImage = SpriteCreator::recolorImageWithTable(captureImage, table);
        }
    }
    else if (mode == GameEnums::Recoloring_None)
    {
        m_buildingImage = SpriteCreator::mergeImages(m_buildingImage, preCaptureImage);
        m_captureBuildingImage = SpriteCreator::mergeImages(m_captureBuildingImage, captureImage);
    }
}

void GameAnimationCapture::createBuildingActor(oxygine::ResAnim* pAnim, Player* startPlayer, Player* capturedPlayer)
{
    spGameMap pMap = GameMap::getInstance();
    if (m_BuildingActor.get() == nullptr &&
        pMap.get() != nullptr)
    {
        m_BuildingActor = oxygine::spActor::create();
        float endPoints = m_endPoints;
        if (m_endPoints == 0)
        {
            endPoints = m_maxPoints;
        }
        float startPercent = static_cast<float>(m_startPoints) / static_cast<float>(m_maxPoints);
        float percentDone = static_cast<float>(endPoints) / static_cast<float>(m_maxPoints);
        float buildingOffsetY = -12;
        float movingHeight = pAnim->getHeight();
        float startPosition = buildingOffsetY + (startPercent) * movingHeight;
        m_BuildingActor->setPosition(0, startPosition);
        m_BuildingActor->setScaleY(1.0f - startPercent);
        oxygine::spTweenQueue queueAnimating = oxygine::spTweenQueue::create();
        oxygine::spTweenQueue queueMoving = oxygine::spTweenQueue::create();
        // going down of the building
        oxygine::timeMS jumpingDuration = oxygine::timeMS(m_frameTime * m_jumpSprites * m_jumpingCount + m_frameTime * m_ayeAyeSprites);
        // dummy tween doing nothing except sync the animation
        oxygine::spTween dummyTween = oxygine::createTween(oxygine::Actor::TweenScaleY(1.0f - startPercent), jumpingDuration, 1);
        dummyTween->setDoneCallback([=](oxygine::Event *)
        {
            Mainapp::getInstance()->getAudioThread()->playSound("capture_down.wav");
        });
        queueAnimating->add(dummyTween);
        oxygine::spTween tween = oxygine::createTween(oxygine::Actor::TweenScaleY(1.0f - percentDone), oxygine::timeMS(m_capturingFactor * m_frameTime), 1);
        queueAnimating->add(tween);
        // dummy tween doing nothing except sync the animation
        oxygine::spTween dummyTween2 = oxygine::createTween(oxygine::Actor::TweenY(startPosition), jumpingDuration, 1);
        queueMoving->add(dummyTween2);
        oxygine::spTween tween2 = oxygine::createTween(oxygine::Actor::TweenY(buildingOffsetY + percentDone * movingHeight), oxygine::timeMS(m_capturingFactor * m_frameTime), 1);
        queueMoving->add(tween2);
        if (m_endPoints == 0)
        {
            tween2->addDoneCallback([=](oxygine::Event *)
            {
                m_buildingSprites->setResAnim(m_captureBuildingResAnim.get());
                if (pMap->getCurrentViewPlayer()->isEnemy(capturedPlayer))
                {
                    Mainapp::getInstance()->getAudioThread()->playSound("capture_enemy.wav");
                }
                else
                {
                    Mainapp::getInstance()->getAudioThread()->playSound("capture_ally.wav");
                }
            });
            oxygine::spTween tween3 = oxygine::createTween(oxygine::Actor::TweenScaleY(1.0f), oxygine::timeMS(m_capturingFactor * m_frameTime), 1, false);
            queueAnimating->add(tween3);

            oxygine::spTween tween4 = oxygine::createTween(oxygine::Actor::TweenY(buildingOffsetY), oxygine::timeMS(m_capturingFactor * m_frameTime), 1, false);
            queueMoving->add(tween4);
        }
        m_BuildingActor->addTween(queueAnimating);
        m_BuildingActor->addTween(queueMoving);
        addChild(m_BuildingActor);
    }
}

void GameAnimationCapture::addSoldierSprite(QString spriteID, Player*  pPlayer, GameEnums::Recoloring mode)
{
    GameAnimationManager* pGameAnimationManager = GameAnimationManager::getInstance();
    oxygine::ResAnim* pAnim = pGameAnimationManager->getResAnim(spriteID);
    if (pAnim != nullptr)
    {
        float endPoints = m_endPoints;
        if (m_endPoints == 0)
        {
            endPoints = m_maxPoints;
        }
        float unitOffsetY = -pAnim->getHeight() - 8 + m_BuildingStartPos;
        float buildingHigh = m_BuildingEndPos - m_BuildingStartPos;
        float percentDone = static_cast<float>(endPoints) / static_cast<float>(m_maxPoints);
        float startPosition = unitOffsetY + (static_cast<float>(m_startPoints) / static_cast<float>(m_maxPoints)) * buildingHigh;
        oxygine::spSprite pSprite = oxygine::spSprite::create();
        if (pAnim->getTotalFrames() > 1)
        {
            // jumping
            oxygine::spTweenQueue queueAnimating = oxygine::spTweenQueue::create();
            oxygine::spTween tween = oxygine::createTween(oxygine::TweenAnim(pAnim, 0, m_jumpSprites - 1), oxygine::timeMS(m_jumpSprites * m_frameTime), m_jumpingCount);
            for (qint32 i = 0; i < m_jumpingCount; ++i)
            {
                addSound("capture_jump.wav", 1, "resources/sounds/", m_jumpSprites * m_frameTime * i);
            }
            queueAnimating->add(tween);
            // cool aye aye sir
            oxygine::spTween tween2 = oxygine::createTween(oxygine::TweenAnim(pAnim, m_jumpSprites, m_jumpSprites + m_ayeAyeSprites - 1), oxygine::timeMS(m_ayeAyeSprites * m_frameTime), 1);
            queueAnimating->add(tween2);
            // going down on the building
            float endPosition = unitOffsetY + percentDone * buildingHigh;
            oxygine::spTween tween3 = oxygine::createTween(oxygine::Actor::TweenY(endPosition), oxygine::timeMS(m_capturingFactor * m_frameTime), 1);
            queueAnimating->add(tween3);
            // maybe going up again as well
            if (m_endPoints == 0)
            {
                oxygine::spTween tween4 = oxygine::createTween(oxygine::Actor::TweenY(unitOffsetY), oxygine::timeMS(m_capturingFactor * m_frameTime), 1);
                queueAnimating->add(tween4);
                oxygine::spTween tween5 = oxygine::createTween(oxygine::Actor::TweenY(unitOffsetY), oxygine::timeMS(m_frameTime), 1);
                queueAnimating->add(tween5);
                if(!m_finishQueued)
                {
                    m_finishQueued = true;
                    tween5->addDoneCallback([=](oxygine::Event *)->void
                    {
                        emitFinished();
                    });
                }
            }
            else
            {
                if(!m_finishQueued)
                {
                    m_finishQueued = true;
                    tween3->addDoneCallback([=](oxygine::Event *)->void
                    {
                        emitFinished();
                    });
                }
            }

            pSprite->addTween(queueAnimating);
        }
        else
        {
            pSprite->setResAnim(pAnim);
        }

        if (mode == GameEnums::Recoloring_Mask)
        {
            pSprite->setColor(pPlayer->getColor());
            pSprite->setPriority(20);
        }
        else if (mode == GameEnums::Recoloring_Table)
        {
            pSprite->setColorTable(pPlayer->getColorTableAnim());
            pSprite->setPriority(20);
        }
        else
        {
            pSprite->setPriority(21);
        }
        pSprite->setPosition(12, startPosition);
        addChild(pSprite);
    }
    else
    {
        Console::print("Unable to load animation sprite: " + spriteID, Console::eERROR);
    }
}

void GameAnimationCapture::addBackgroundSprite(QString spriteID)
{
    GameAnimationManager* pGameAnimationManager = GameAnimationManager::getInstance();
    oxygine::ResAnim* pAnim = pGameAnimationManager->getResAnim(spriteID);
    if (pAnim != nullptr)
    {
        oxygine::spSprite pSprite = oxygine::spSprite::create();
        pSprite->setResAnim(pAnim);
        pSprite->setPriority(0);
        addChild(pSprite);
    }
    else
    {
        Console::print("Unable to load animation sprite: " + spriteID, Console::eERROR);
    }
}
