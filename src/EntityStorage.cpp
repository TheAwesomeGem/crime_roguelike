#include "EntityStorage.h"


EntityStorage::EntityStorage()
        : Entities{} {
}


Entity* EntityStorage::Create(EntityId Id) {
    auto EntityIt = Entities.emplace(Id, Id);

    return &EntityIt.first->second;
}

bool EntityStorage::Destroy(EntityId Id) {
    auto EntityIt = Entities.find(Id);

    if (EntityIt != Entities.end()) {
        Entity& EntityToDestroy = EntityIt->second;
        EntityToDestroy.Id = uuids::uuid{};

        for (auto& [_, ComponentToDestroy]: EntityToDestroy.Components) {
            RemoveComponent(&EntityToDestroy, ComponentToDestroy);
        }

        Entities.erase(EntityIt);

        return true;
    }

    return false;
}

Entity* EntityStorage::GetEntityFromId(EntityId IdToGet) {
    auto EntityIt = Entities.find(IdToGet);

    if (EntityIt == Entities.end()) {
        return nullptr;
    }

    return &EntityIt->second;
}
