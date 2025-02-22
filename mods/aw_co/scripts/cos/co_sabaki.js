CO_SABAKI.init = function(co, map)
{
    co.setPowerStars(0);
    co.setSuperpowerStars(3);
};
CO_SABAKI.activateSuperpower = function(co, powerMode, map)
{
    CO_SABAKI.activatePower(co, powerMode, map);
};
CO_SABAKI.getSuperPowerDescription = function()
{
    return CO_SABAKI.getPowerDescription();
};
CO_SABAKI.getSuperPowerName = function()
{
    return CO_SABAKI.getPowerName();
};
CO_SABAKI.postBattleActions = function(co, attacker, atkDamage, defender, gotAttacked, weapon, action, map)
{
    if (co.getIsCO0() === true)
    {
        if (gotAttacked === false && attacker.getOwner() === co.getOwner())
        {
            var healPercent = 0.0;
            switch (co.getPowerMode())
            {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
            case GameEnums.PowerMode_Power:
                healPercent = 0.5;
                break;
            default:
                healPercent = 0.15;
                break;

            }
            // damage can be negativ if we can't do a counter attack the damge is -1
            // avoid loosing hp cause of our passive or power
            if (atkDamage > 0)
            {
                attacker.setHp(attacker.getHp() + atkDamage * healPercent);
            }
        }
    }
};

CO_SABAKI.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                      defender, defPosX, defPosY, isDefender, action, luckmode, map)
{
    return 0;
};

CO_SABAKI.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                       defender, defPosX, defPosY, isAttacker, action, luckmode, map)
{
    if (co.getIsCO0() === true)
    {
        if (co.getPowerMode() > GameEnums.PowerMode_Off)
        {
            return 10;
        }
    }
    return 0;
};
