#ifndef BATTLEANIMATIONSPRITE_H
#define BATTLEANIMATIONSPRITE_H

#include <QObject>
#include <QTimer>


#include "3rd_party/oxygine-framework/oxygine/actor/Sprite.h"
#include "3rd_party/oxygine-framework/oxygine/actor/ClipRectActor.h"

#include "game/unit.h"
#include "game/gamemap.h"

class BattleAnimationSprite;
using spBattleAnimationSprite = oxygine::intrusive_ptr<BattleAnimationSprite>;

class BattleAnimationSprite final : public QObject, public oxygine::Sprite
{
    Q_OBJECT
public:
    static const QString standingAnimation;
    static const QString impactUnitOverlayAnimation;
    static const QString impactAnimation;
    static const QString fireAnimation;
    static const QString moveInAnimation;
    static const QString standingFiredAnimation;
    static const QString dyingAnimation;
    static const QString stopAnimation;

    explicit BattleAnimationSprite(GameMap* pMap, spUnit pUnit, Terrain* pTerrain, QString animationType, qint32 hp = -1, bool playSound = true);
    ~BattleAnimationSprite();
    /**
     * @brief loadAnimation
     * @param animationType
     */
    void loadAnimation(QString animationType);

    /**
     * @brief getClipActor
     * @return
     */
    oxygine::spClipRectActor getClipActor()
    {
        return m_Actor;
    }
    /**
     * @brief clear removes all sprites from the animation
     */
    void clear();
    /**
     * @brief flipActorsX
     * @param flippedX
     */
    virtual void flipActorsX(bool flippedX) override;
    /**
     * @brief getStartWithFraming
     * @return
     */
    bool getStartWithFraming() const;
    /**
     * @brief setStartWithFraming
     * @param startWithFraming
     */
    void setStartWithFraming(bool startWithFraming);
    /**
     * @brief startNextFrame
     */
    void startNextFrame();
    /**
     * @brief setDyingStartHp
     * @param dyingStartHp
     */
    void setDyingStartHp(float dyingStartHp);
    /**
     * @brief setDyingEndHp
     * @param dyingEndHp
     */
    void setDyingEndHp(float dyingEndHp);
    /**
     * @brief setFireHp
     * @param fireHp
     */
    void setFireHp(float fireHp)
    {
        m_fireHp = fireHp;
    }
    /**
     * @brief setEnemySprite
     * @param newEnemySprite
     */
    void setEnemySprite(BattleAnimationSprite *newEnemySprite);
    /**
     * @brief setBackgroundSprite
     * @param newPBackgroundSprite
     */
    void setBackgroundSprite(oxygine::spSprite newBackgroundSprite);
    /**
     * @brief setHasFired
     * @param newHasFired
     */
    void setHasFired(bool newHasFired);
    /**
     * @brief getPlayNextFrame
     * @return
     */
    bool getPlayNextFrame() const;
    /**
     * @brief setPlayNextFrame
     * @param newPlayNextFrame
     */
    void setPlayNextFrame(bool newPlayNextFrame);


signals:
    void sigDetachChild(oxygine::spActor pActor);
public slots:
    /**
     * @brief getMap
     * @return
     */
    GameMap *getMap() const;
    /**
     * @brief existResAnim
     * @param spriteID
     * @return checks if either a battleanimation sprite or co-sprite with this name exists
     */
    bool existResAnim(QString spriteID);
    /**
     * @brief getHasFired
     * @return
     */
    bool getHasFired() const;
    /**
     * @brief getBackgroundSpeed
     * @return
     */
    float getBackgroundSpeed();
    /**
     * @brief setBackgroundSpeed
     * @param speed
     */
    void setBackgroundSpeed(float speed);
    /**
     * @brief restoreBackgroundSpeed
     */
    void restoreBackgroundSpeed();
    /**
     * @brief setInvertStartPosition
     * @param invertStartPosition
     */
    void setInvertStartPosition(bool invertStartPosition);
    /**
     * @brief getEnemySprite
     * @return
     */
    BattleAnimationSprite *getEnemySprite() const;
    /**
     * @brief loadDyingFadeOutAnimation
     */
    qint32 loadDyingFadeOutAnimation(qint32 fadeoutTime);
    /**
     * @brief addMoveTweenToLastLoadedSprites
     * @param deltaX
     * @param deltaY
     * @param moveTime
     * @param loops
     */
    void addMoveTweenToLastLoadedSprites(qint32 deltaX, qint32 deltaY, qint32 moveTime, qint32 delayPerUnitMs = 75, qint32 loops = -1, bool scaleWithAnimationSpeed = false);
    /**
     * @brief loadColorOverlayForLastLoadedFrame
     * @param color
     * @param time
     * @param loops
     * @param showDelayMs
     */
    void loadColorOverlayForLastLoadedFrame(QColor color, qint32 time, qint32 loops, qint32 showDelayMs);
    /**
     * @brief addUnitshakeToLastLoadedFrame
     * @param startIntensity
     * @param decay
     * @param durationMs
     * @param delayMs
     * @param shakePauseMs
     */
    void addUnitshakeToLastLoadedFrame(qint32 startIntensity, float decay, qint32 durationMs, qint32 delayMs = 0, qint32 shakePauseMs = 30);
    /**
     * @brief getInvertStartPosition
     * @return is true during impact animations and results in an inverted positioning of the impacts.
     * So the impacts are drawm at the position of the dying units
     */
    bool getInvertStartPosition() const;
    /**
     * @brief getDyingStartHp
     * @return
     */
    float getDyingStartHp() const;
    /**
     * @brief getDyingEndHp
     * @return
     */
    float getDyingEndHp() const;

    /**
     * @brief setMaxUnitCount
     * @param value
     */
    void setMaxUnitCount(const qint32 &value);
    /**
     * @brief loadAnimation
     * @param animationType animation type we want to load
     * @param pUnit attacking unit
     * @param pDefender defending unit
     * @param attackerWeapon weapon used by the attacking unit
     * @param clearSprite if true clears the battle animation sprite buffer so you can add stuff from scratch default
     */
    void loadAnimation(QString animationType, Unit* pUnit, Unit* pDefender = nullptr, qint32 attackerWeapon = 0, bool clearSprite = true, bool start = true);
    /**
     * @brief getMaxUnitCount
     * @return
     */
    qint32 getMaxUnitCount();
    /**
     * @brief getUnitCount
     * @param maxUnitCount
     * @return returns the unit count based on the hp of the current unit
     */
    qint32 getUnitCount(qint32 maxUnitCount);
    /**
     * @brief getFireUnitCount
     * @param maxUnitCount
     * @return
     */
    qint32 getFireUnitCount(qint32 maxUnitCount);
    /**
     * @brief getAnimationUnitCount
     * @return
     */
    qint32 getAnimationUnitCount();
    /**
     * @brief getUnitCount
     * @param maxUnitCount
     * @param hp
     * @return
     */
    qint32 getUnitCount(qint32 maxUnitCount, qint32 hp);
    /**
     * @brief getHpRounded
     * @return hp rounded of the unit
     */
    qint32 getHpRounded() const;
    /**
     * @brief setHpRounded
     * @param value simulated hp for the sprite
     */
    void setHpRounded(const qint32 &value);
    /**
     * @brief getUnitPositionOffset
     * @param unitIdx
     * @return calculates the position offset based on the unit model and the current terrain -> see mountains
     */
    QPoint getUnitPositionOffset(qint32 unitIdx);
    /**
     * @brief getUnitPosition
     * @param unitCount
     * @return calculates the position offset based on the unit model number
     */
    QPoint getUnitPosition(qint32 unitCount, qint32 maxUnitCount);
    /**
     * @brief loadSprite loads a standing animated sprite for every shown model
     * @param spriteID the sprite resource which should be loaded
     * @param addPlayerColor apply the player color to this sprite or not
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     * @param deleteAfter deletes the sprite after finished movement or animation
     */
    void loadSprite(QString spriteID, bool addPlayerColor, qint32 maxUnitCount, QPoint offset,
                    qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                    bool _invertFlipX = false, bool deleteAfter = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1, qint32 startFrame = 0);
    /**
     * @brief loadSprite loads a standing animated sprite for every shown model
     * @param spriteID the sprite resource which should be loaded
     * @param mode how the player color is applied to the sprite
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     * @param deleteAfter deletes the sprite after finished movement or animation
     */
    void loadSpriteV2(QString spriteID, GameEnums::Recoloring mode, qint32 maxUnitCount, QPoint offset,
                      qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                      bool _invertFlipX = false, bool deleteAfter = false, qint32 frameTime = GameMap::frameTime,
                      qint32 frames = -1, qint32 startFrame = 0);
    /**
     * @brief loadMovingSprite loads a moving sprite for every shown model
     * @param spriteID the sprite resource which should be loaded
     * @param addPlayerColor apply the player color to this sprite or not
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param movement movement of the animation as point. Amount of pixels the sprite should move.
     * @param moveTime time to reach the target
     * @param deleteAfter deletes the sprite after finished movement or animation
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     */
    void loadMovingSprite(QString spriteID, bool addPlayerColor, qint32 maxUnitCount, QPoint offset,
                          QPoint movement, qint32 moveTime, bool deleteAfter = false,
                          qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                          bool _invertFlipX = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1, qint32 startFrame = 0);
    /**
     * @brief loadMovingSprite loads a moving sprite for every shown model
     * @param spriteID the sprite resource which should be loaded
     * @param mode how the player color is applied to the sprite
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param movement movement of the animation as point. Amount of pixels the sprite should move.
     * @param moveTime time to reach the target
     * @param deleteAfter deletes the sprite after finished movement or animation
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     */
    void loadMovingSpriteV2(QString spriteID, GameEnums::Recoloring mode, qint32 maxUnitCount, QPoint offset,
                            QPoint movement, qint32 moveTime, bool deleteAfter = false,
                            qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                            bool _invertFlipX = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1, qint32 startFrame = 0);

    /**
     * @brief loadDyingMovingSprite
     * @param livingSpriteId
     * @param dyingSpriteId
     * @param mode
     * @param offset
     * @param movement
     * @param rotation
     * @param moveTime
     * @param priority
     * @param showDelay
     */
    void loadDyingMovingSprite(QString livingSpriteId, QString dyingSpriteId, GameEnums::Recoloring mode, QPoint offset,
                               QPoint movement = QPoint(0, 0), float rotation = 0, qint32 moveTime = 0, short priority = 0, qint32 firedFrame = 0, qint32 maxUnitCount = 5, qint32 showDelay = 0);
    /**
     * @brief loadOnlyDyingMovingSprite
     * @param dyingSpriteId
     * @param mode
     * @param offset
     * @param movement
     * @param rotation
     * @param moveTime
     * @param priority
     * @param firedFrame
     * @param maxUnitCount
     * @param showDelay
     */
    void loadOnlyDyingMovingSprite(QString dyingSpriteId, GameEnums::Recoloring mode, QPoint offset,
                                   quint8 alpha = 180, QPoint movement = QPoint(0, 0), float rotation = 0, qint32 moveTime = 0,
                                   short priority = 0, qint32 firedFrame = 0, qint32 maxUnitCount = 5, qint32 showDelay = 0);
    /**
     * @brief loadSingleMovingSprite loads a single sprite for a unit
     * @param spriteID the sprite resource which should be loaded
     * @param addPlayerColor apply the player color to this sprite or not
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param movement movement of the animation as point
     * @param moveTime time to reach the target
     * @param deleteAfter deletes the sprite after finished movement or animation
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     */
    void loadSingleMovingSprite(QString spriteID, bool addPlayerColor, QPoint offset,
                                QPoint movement, qint32 moveTime, bool deleteAfter = false,
                                qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                                bool _invertFlipX = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1,
                                qint32 startFrame = 0, float rotation = 0, quint8 alpha = 255);

    /**
     * @brief loadSingleMovingSprite loads a single sprite for a unit
     * @param spriteID the sprite resource which should be loaded
    * @param mode how the player color is applied to the sprite
     * @param maxUnitCount maximum unit count. Needed to create the amount of models of this sprite based on the units hp
     * @param offset offset from the original model position
     * @param movement movement of the animation as point
     * @param moveTime time to reach the target
     * @param deleteAfter deletes the sprite after finished movement or animation
     * @param loop the amount of loops played for an animated sprite
     * @param scale scale of the sprite 1.0 -> no scaling
     * @param priority priority order in which this sprite will be shown
     * @param showDelay delay before showing this sprite
     * @param invertFlipX if true the flipping of the sprite is inverted needed for impacts of rockets etc.
     */
    void loadSingleMovingSpriteV2(QString spriteID, GameEnums::Recoloring mode, QPoint offset,
                                  QPoint movement, qint32 moveTime, bool deleteAfter = false,
                                  qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                                  bool _invertFlipX = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1, qint32 startFrame = 0,
                                  float rotation = 0, quint8 alpha = 255);
    /**
     * @brief loadCoMini
     * @param spriteID
     * @param mode
     * @param offset
     * @param movement
     * @param moveTime
     * @param deleteAfter
     * @param loop
     * @param scale
     * @param priority
     * @param showDelay
     * @param _invertFlipX
     * @param frameTime
     * @param frames
     * @param startFrame
     * @param rotation
     * @param alpha
     */
    void loadCoMini(QString spriteID, GameEnums::Recoloring mode, QPoint offset,
                    QPoint movement, qint32 moveTime, bool deleteAfter = false,
                    qint32 loop = 1, float scale = 1.0f, short priority = 0, qint32 showDelay = 0,
                    bool _invertFlipX = false, qint32 frameTime = GameMap::frameTime, qint32 frames = -1, qint32 startFrame = 0,
                    float rotation = 0, quint8 alpha = 255);
    /**
     * @brief getImpactDurationMS
     * @return
     */
    qint32 getImpactDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getFireDurationMS
     * @return
     */
    qint32 getFireDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getFiredDurationMS
     * @param pUnit
     * @param pDefender
     * @param attackerWeapon
     * @return
     */
    qint32 getFiredDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getMoveInDurationMS
     * @return
     */
    qint32 getMoveInDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getStopDurationMS
     * @return
     */
    qint32 getStopDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getMoveInDurationMS
     * @return
     */
    qint32 getDyingDurationMS(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief getMoveInDurationMS
     * @return
     */
    bool hasDyingAnimation();
    /**
     * @brief hasMoveInAnimation
     * @return
     */
    bool hasMoveInAnimation(Unit* pUnit, Unit* pDefender, qint32 attackerWeapon);
    /**
     * @brief detachChild
     * @param pActor
     */
    void detachChild(oxygine::spActor pActor);
    /**
     * @brief loadSound
     * @param file sound file
     * @param loops amount of loops for this file -2 for while this animation is shown
     * @param folder resource folder of the sound
     * @param delay before this sound is played after this function was called
     */
    void loadSound(QString file, qint32 loops, qint32 delay = 0, float volume = 1.0f, bool stopOldestSound = false);
    /**
     * @brief stopSound
     */
    void stopSound(bool forceStop = false);
    /**
     * @brief setUnitFrameDelay delay time between each individual unit animation start
     */
    void setUnitFrameDelay(qint32 delay);
    /**
     * @brief addScreenshake
     * @param startIntensity
     * @param decay
     * @param durationMs
     * @param shakePauseMs
     */
    void addBattleViewScreenshake(qint32 startIntensity, float decay, qint32 durationMs, qint32 delayMs = 0, qint32 shakePauseMs = 30);
    /**
     * @brief addScreenshake
     * @param startIntensity
     * @param decay
     * @param durationMs
     * @param delayMs
     * @param shakePauseMs
     */
    void addScreenshake(qint32 startIntensity, float decay, qint32 durationMs, qint32 delayMs = 0, qint32 shakePauseMs = 30);
    /**
     * @brief addSpriteScreenshake
     * @param startIntensity
     * @param decay
     * @param durationMs
     * @param delayMs
     * @param shakePauseMs
     */
    void addSpriteScreenshake(qint32 startIntensity, float decay, qint32 durationMs, qint32 delayMs = 0, qint32 shakePauseMs = 30);
    /**
     * @brief addSpriteShakeY
     * @param startIntensity
     * @param startPercent
     * @param endPercent
     * @param durationMs
     * @param delayMs
     */
    void addSpriteShakeY(qint32 startIntensity, float startPercent, float endPercent, qint32 durationMs, qint32 delayMs, qint32 loops);
    /**
     * @brief getUnitBasePosition
     * @param unit
     * @param maxUnitCount
     * @param unitsAlive
     * @return
     */
    QPoint getUnitBasePosition(qint32 unit, qint32 maxUnitCount, qint32 unitsAlive);
private slots:
    void startNextUnitFrames();
private:
    /**
     * @brief loadSpriteInternal
     * @param pAnim
     * @param mode
     * @param offset
     * @param movement
     * @param moveTime
     * @param deleteAfter
     * @param loops
     * @param scale
     * @param priority
     * @param showDelay
     * @param _invertFlipX
     * @param frameTime
     * @param frames
     * @param startFrame
     * @param rotation
     * @param alpha
     */
    void loadSpriteInternal(oxygine::ResAnim* pAnim, GameEnums::Recoloring mode, QPoint offset,
                            QPoint movement, qint32 moveTime, bool deleteAfter,
                            qint32 loops, float scale, short priority, qint32 showDelay,
                            bool _invertFlipX, qint32 frameTime, qint32 frames, qint32 startFrame,
                            float rotation, quint8 alpha);
private:
    struct SoundData
    {
        QString sound;
        qint32 loops;
    };
    spUnit m_pUnit;
    Terrain* m_pTerrain;
    oxygine::spClipRectActor m_Actor;
    qint32 m_hpRounded{0};
    qint32 m_maxUnitCount{-1};


    QVector<SoundData> m_Sounds;

    QVector<QVector<oxygine::spSprite>> m_currentFrame;

    QVector<QVector<QVector<oxygine::spSprite>>> m_nextFrames;
    oxygine::spSprite m_lastLoadedSprite;
    qint32 m_frameIterator{0};
    QTimer m_nextFrameTimer;
    bool m_startWithFraming{false};

    BattleAnimationSprite* m_EnemySprite{nullptr};

    float m_dyingStartHp{10.0f};
    float m_dyingEndHp{10.0f};
    float m_fireHp{10.0f};
    bool m_invertStartPosition{false};
    bool m_playSound{true};
    bool m_hasFired{false};
    bool m_playNextFrame{false};

    oxygine::spSprite m_pBackgroundSprite;
    float m_backgroundSpeed{0.0f};
    GameMap* m_pMap{nullptr};
};

Q_DECLARE_INTERFACE(BattleAnimationSprite, "BattleAnimationSprite");

#endif // BATTLEANIMATIONSPRITE_H
