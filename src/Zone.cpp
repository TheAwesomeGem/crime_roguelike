#include "Zone.h"


Zone::Zone(Point ZoneCoordinate)
        : Coordinate{ZoneCoordinate}, TurnsSinceLastCreated{-1}, NPCEntities{}, Components{} {

}