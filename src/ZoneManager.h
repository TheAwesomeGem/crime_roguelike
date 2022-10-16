#pragma once

#include "Common.h"
#include "GameState.h"


// TODO: Should this be an object inside World?
namespace ZoneManager {
    Zone* GetZoneFromCoord(World* World, Point ZoneCoordinate);

    void ClearZone(GameState* Game, World* World, Zone* ZoneToClear);

    void RegenerateZone(GameState* Game, World* World, Zone* ZoneToRegenerate);

    void ChangedZone(GameState* Game, Entity* Entity, Point OldZoneCoord, Point NewZoneCoord);
}
