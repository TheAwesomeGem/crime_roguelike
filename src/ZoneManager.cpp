#include "ZoneManager.h"


Zone* ZoneManager::GetZoneFromCoord(World* World, Point ZoneCoordinate) {
    auto ZoneIt = World->Zones.find(ZoneCoordinate);

    if (ZoneIt == World->Zones.end()) {
        return nullptr;
    }

    return &ZoneIt->second;
}

void ZoneManager::ClearZone(GameState* Game, World* World, Zone* ZoneToClear) {
    printf("Zone cleared: [%d, %d, %d]\n", ZoneToClear->Coordinate.X, ZoneToClear->Coordinate.Y, ZoneToClear->Coordinate.Z);
    for (EntityId IdToDestroy: ZoneToClear->NPCEntities) {
        Entity* EntityToDestroy = World->GetEntity(IdToDestroy);

        World->DestroyEntity(EntityToDestroy);

    }

    ZoneToClear->NPCEntities.clear();
}

void ZoneManager::RegenerateZone(GameState* Game, World* World, Zone* ZoneToRegenerate) {
    printf("Zone regenerated: [%d, %d, %d]\n", ZoneToRegenerate->Coordinate.X, ZoneToRegenerate->Coordinate.Y, ZoneToRegenerate->Coordinate.Z);
    for (int i = 0; i < 3; ++i) {
        auto [_, SpawnedEntity] = Game->CreateEntityWithController(ZoneToRegenerate->Coordinate);
        SpawnedEntity->SetCoordCurrent();

        ZoneToRegenerate->NPCEntities.push_back(SpawnedEntity->Id);
    }
}

void ZoneManager::ChangedZone(GameState* Game, Entity* Entity, Point OldZoneCoord, Point NewZoneCoord) {
    printf("Entity moved from [%d, %d, %d] to [%d, %d, %d]\n", OldZoneCoord.X, OldZoneCoord.Y, OldZoneCoord.Z, NewZoneCoord.X, NewZoneCoord.Y, NewZoneCoord.Z);
    Zone* OldZone = GetZoneFromCoord(&Game->CurrentWorld, OldZoneCoord);
    Zone* NewZone = GetZoneFromCoord(&Game->CurrentWorld, NewZoneCoord);

    NewZone->PlayableEntitiesCache.emplace(Entity->Id);

    if (OldZone != nullptr) {
        OldZone->PlayableEntitiesCache.erase(Entity->Id);
    }

    if (!NewZone->IsGenerated()) {
        RegenerateZone(Game, &Game->CurrentWorld, NewZone);
    }

    if (OldZone != nullptr && OldZone->ShouldDestroy()) {
        ClearZone(Game, &Game->CurrentWorld, OldZone);
    }
}
