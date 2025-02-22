var Constructor = function()
{
    this.init = function (building)
    {
        building.setAlwaysVisble(true);
    };
    this.loadSprites = function(building, neutral, map)
    {
        building.loadSprite("nest_mortar_left", false);
    };
    this.getBaseIncome = function()
    {
        return 0;
    };
    this.getDefense = function()
    {
        return 0;
    };
    this.getMiniMapIcon = function()
    {
        return "minimap_blackholebuilding";
    };
    this.getName = function()
    {
        return qsTr("Nest");
    };
    this.getDescription = function()
    {
        return qsTr("Nest of Dark Matter. Currently only a visual.");
    };
    this.getBuildingWidth = function()
    {
        return 6;
    };
    this.getBuildingHeigth = function()
    {
        return 6;
    };
    this.canBuildingBePlaced = function(terrain, building, map)
    {
        return BUILDING.canLargeBuildingPlaced(terrain, building, ZNEST_LEFT_MORTAR.getBuildingWidth(), ZNEST_LEFT_MORTAR.getBuildingHeigth(), map);
    };
    this.getDamage = function(building, unit)
    {
        return 5;
    };
    this.getActionTargetFields = function(building)
    {
        var targets = globals.getEmptyPointArray();
        // laser to not fire infinitly but the range is still fucking huge :)
        for (var i = 0; i < 60; i++)
        {
            for (var i2 = 0; i2 < 7; i2++)
            {
                targets.append(Qt.point(-6 + i2, -30 + i));
            }
        }
        return targets;
    };
    this.getAbsoluteActionTargetFields = function(building)
    {
        var x = building.getX();
        var y = building.getY();
        var targets = globals.getEmptyPointArray();
        // laser to not fire infinitly but the range is still fucking huge :)
        for (var i = 0; i < 60; i++)
        {
            for (var i2 = 0; i2 < 7; i2++)
            {
                targets.append(Qt.point(-6 + i2 + x, -30 + i + y));
            }
        }
        return targets;
    };
    this.actionList = ["ACTION_NEST_MORTAR"];
    this.startOfTurn = function(building, map)
    {
        building.setFireCount(building.getFireCount() + 1);
    };
}

Constructor.prototype = BUILDING;
var ZNEST_LEFT_MORTAR = new Constructor();
