var Constructor = function()
{
    this.init = function(co, map)
    {
        co.setPowerStars(3);
        co.setSuperpowerStars(4);
    };

    this.getCOStyles = function()
    {
        // string array containing the endings of the alternate co style
        
        return ["+alt"];
    };

    this.activatePower = function(co, map)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(GameEnums.PowerMode_Power);
        dialogAnimation.queueAnimation(powerNameAnimation);

        var units = co.getOwner().getUnits();
        var animations = [];
        var counter = 0;
        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);
            var animation = GameAnimationFactory.createAnimation(map, unit.getX(), unit.getY());
            var delay = globals.randInt(135, 265);
            if (animations.length < 5)
            {
                delay *= i;
            }
            if (i % 2 === 0)
            {
                animation.setSound("power8_1.wav", 1, delay);
            }
            else
            {
                animation.setSound("power8_2.wav", 1, delay);
            }
            if (animations.length < 5)
            {
                animation.addSprite("power8", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 2, delay);
                powerNameAnimation.queueAnimation(animation);
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power8", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 2, delay);
                animations[counter].queueAnimation(animation);
                animations[counter] = animation;
                counter++;
                if (counter >= animations.length)
                {
                    counter = 0;
                }
            }
        }
        units.remove();
    };

    this.activateSuperpower = function(co, powerMode, map)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(powerMode);
        powerNameAnimation.queueAnimationBefore(dialogAnimation);

        CO_CASSIDY.cassidyDamage(co, 1, dialogAnimation, map);
    };

    this.cassidyDamage = function(co, value, animation2, map)
    {
        var player = co.getOwner();
        var counter = 0;
        var playerCounter = map.getPlayerCount();
        var animation = null;
        var animations = [];

        for (var i2 = 0; i2 < playerCounter; i2++)
        {
            var enemyPlayer = map.getPlayer(i2);
            if ((enemyPlayer !== player) &&
                    (player.checkAlliance(enemyPlayer) === GameEnums.Alliance_Enemy))
            {

                var units = enemyPlayer.getUnits();
                units.randomize();
                for (i = 0; i < units.size(); i++)
                {
                    var unit = units.at(i);

                    animation = GameAnimationFactory.createAnimation(map, unit.getX(), unit.getY());
                    var delay = globals.randInt(135, 265);
                    if (animations.length < 5)
                    {
                        delay *= i;
                    }
                    animation.setSound("power4.wav", 1, delay);
                    if (animations.length < 5)
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 2, delay);
                        animation2.queueAnimation(animation);
                        animations.push(animation);
                    }
                    else
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 2, delay);
                        animations[counter].queueAnimation(animation);
                        animations[counter] = animation;
                        counter++;
                        if (counter >= animations.length)
                        {
                            counter = 0;
                        }
                    }
                    animation.writeDataInt32(unit.getX());
                    animation.writeDataInt32(unit.getY());
                    animation.writeDataInt32(value);
                    animation.setEndOfAnimationCall("ANIMATION", "postAnimationDamage");
                }
                units.remove();
            }
        }
    };

    this.loadCOMusic = function(co, map)
    {
        // put the co music in here.
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Power:
            audio.addMusic("resources/music/cos/bh_power.mp3", 1091 , 49930);
            break;
        case GameEnums.PowerMode_Superpower:
            audio.addMusic("resources/music/cos/bh_superpower.mp3", 3161 , 37731);
            break;
        case GameEnums.PowerMode_Tagpower:
            audio.addMusic("resources/music/cos/bh_tagpower.mp3", 779 , 51141);
            break;
        default:
            audio.addMusic("resources/music/cos/cassidy.mp3", 76, 75279)
            break;
        }
    };

    this.getCOUnitRange = function(co, map)
    {
        return 2;
    };
    this.getCOArmy = function()
    {
        return "TI";
    };
    this.coZoneBonus = 40;
    this.defaultPowerBonus = 30;
    this.powerBonus = 60;
    this.superPowerBonus = 80;
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                      defender, defPosX, defPosY, isDefender, action, luckmode, map)
    {
        if (defender !== null)
        {
            switch (co.getPowerMode())
            {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                if (attacker.getHp() >= defender.getHp())
                {
                    return CO_CASSIDY.superPowerBonus;
                }
                return CO_CASSIDY.defaultPowerBonus;
            case GameEnums.PowerMode_Power:
                if (attacker.getHp() >= defender.getHp())
                {
                    return CO_CASSIDY.powerBonus;
                }
                return CO_CASSIDY.defaultPowerBonus;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker))
                {
                    if (attacker.getHp() >= defender.getHp())
                    {
                        return CO_CASSIDY.coZoneBonus;
                    }
                    return 10;
                }
                break;
            }
        }
        return 0;
    };

    this.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                       defender, defPosX, defPosY, isAttacker, action, luckmode, map)
    {
        if (co.inCORange(Qt.point(defPosX, defPosY), defender) ||
                co.getPowerMode() > GameEnums.PowerMode_Off)
        {
            return 10;
        }
        return 0;
    };

    this.getAiCoUnitBonus = function(co, unit, map)
    {
        return 1;
    };

    // CO - Intel
    this.getBio = function(co)
    {
        return qsTr("An extremely belligerent member of Teal Island that is notorious for her ruthlessness. She fights purely for the thrill of battle and is feared by both enemies and allies.");
    };
    this.getHits = function(co)
    {
        return qsTr("Roses");
    };
    this.getMiss = function(co)
    {
        return qsTr("Violets");
    };
    this.getCODescription = function(co)
    {
        return qsTr("Cassidy's ruthlessness rubs off on her troops. Units show no mercy to enemy units equal to or below their own strength.");
    };
    this.getLongCODescription = function()
    {
        var text = qsTr("\nGlobal Effect: \nNone.") +
                qsTr("\n\nCO Zone Effect: \nUnits gain %0% firepower when attacking units with equal or less health.");
        text = replaceTextArgs(text, [CO_CASSIDY.coZoneBonus]);
        return text;

    };
    this.getPowerDescription = function(co)
    {
        var text = qsTr("All of Cassidy's units now gain %0% firepower for attacking units with equal or less health.");
        text = replaceTextArgs(text, [CO_CASSIDY.powerBonus]);
        return text;
    };
    this.getPowerName = function(co)
    {
        return qsTr("Rampage");
    };
    this.getSuperPowerDescription = function(co)
    {
        var text = qsTr("All enemies suffer one HP of damage. Firepower is greatly increased by %0% when attacking a unit with equal or less health.");
        text = replaceTextArgs(text, [CO_CASSIDY.superPowerBonus]);
        return text;
    };
    this.getSuperPowerName = function(co)
    {
        return qsTr("Scorched Earth");
    };
    this.getPowerSentences = function(co)
    {
        return [qsTr("Take no prisoners!"),
                qsTr("Ahahaha! I'll destroy you all!"),
                qsTr("You won't like me when I am angry!"),
                qsTr("I swear to make you rue this day!"),
                qsTr("Burn everything! I don't want to see anything left untouched!"),
                qsTr("I have fury!")];
    };
    this.getVictorySentences = function(co)
    {
        return [qsTr("You better hope we do not meet again."),
                qsTr("I love this job!"),
                qsTr("I wished the battle had lasted a little longer. Oh well.")];
    };
    this.getDefeatSentences = function(co)
    {
        return [qsTr("You will regret this..."),
                qsTr("Kill them... kill them all...")];
    };
    this.getName = function()
    {
        return qsTr("Cassidy");
    };
}

Constructor.prototype = CO;
var CO_CASSIDY = new Constructor();
