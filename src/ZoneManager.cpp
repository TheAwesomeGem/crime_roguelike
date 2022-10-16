#include "ZoneManager.h"
#include "World.h"


Zone* ZoneManager::GetZoneFromCoord(World* World, Point ZoneCoordinate) {
    auto ZoneIt = World->Zones.find(ZoneCoordinate);

    if (ZoneIt == World->Zones.end()) {
        return nullptr;
    }

    return &ZoneIt->second;
}
