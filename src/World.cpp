#include "World.h"
#include "ZoneManager.h"


World::World()
        : Components{}, Entities{}, Zones{} {
    GenerateEmptyZones();
}

Entity* World::SpawnEntity() {
    Entity* SpawnedEntity = Entities.Create(Random::Uuid());

    return SpawnedEntity;
}

void World::DestroyEntity(Entity* EntityToDestroy) {
    ControllerId ControlledById = EntityToDestroy->ControlledBy;
    Entities.Destroy(EntityToDestroy->Id);

    EntityDestroyEvent.Broadcast(EntityToDestroy, ControlledById); // TODO: Figure out if we really need this
}

Entity* World::GetEntity(EntityId IdToGet) {
    return Entities.GetEntityFromId(IdToGet);
}

Zone* World::GetZone(Point ZoneCoord) {
    return ZoneManager::GetZoneFromCoord(this, ZoneCoord);
}

void World::GenerateEmptyZones() {
    for (uint16_t Z = 0; Z < ZONES_DEPTH; ++Z) {
        for (uint16_t Y = 0; Y < ZONES_COLS; ++Y) {
            for (uint16_t X = 0; X < ZONES_ROWS; ++X) {
                Point ZoneCoordinate = Point{X, Y, Z};
                Zones.emplace(ZoneCoordinate, Zone{ZoneCoordinate});
            }
        }
    }
}
