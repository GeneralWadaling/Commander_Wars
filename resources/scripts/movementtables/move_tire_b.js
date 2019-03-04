var Constructor = function()
{
    this.getName = function()
    {
        return qsTr("Tire");
    };
    this.getMovementpoints = function(terrainID)
    {
        switch (terrainID)
        {            
            case "BEACH":
				return 2;            
            case "RUIN":			
			case "DESTROYEDWELD":
			case "PLAINS":
			case "BRIDGE":
            case "STREET":
            case "AIRPORT":
            case "FACTORY":
            case "HARBOUR":
            case "HQ":
            case "MINE":
            case "PIPESTATION":
            case "RADAR":
            case "TOWER":
            case "TOWN":
            case "SILO":
            case "SILO_ROCKET":
            case "LABOR":
                return 1;
			case "FOREST":
			case "WASTELAND":
				return 3;
        }
    };
};
Constructor.prototype = MOVEMENTTABLE;
var MOVE_TIRE_B = new Constructor();
