#pragma once

#include "Common.h"
#include "World.h"


// TODO: Should this be an object inside World?
namespace ZoneManager {
    Zone* GetZoneFromCoord(World* World, Point ZoneCoordinate);
}
