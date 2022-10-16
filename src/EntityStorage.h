#pragma once

#include "Common.h"
#include "Entity.h"


class EntityStorage {
public:
    EntityStorage();

    Entity* Create(EntityId Id);

    bool Destroy(EntityId Id);

    Entity* GetEntityFromId(EntityId IdToGet);

    static void AttachComponent(Entity* Entity, BaseComponent* ComponentToAttach) {
        Entity->Components.emplace(ComponentToAttach->Type, ComponentToAttach);
    }

    static bool RemoveComponent(Entity* Entity, BaseComponent* ComponentToRemove) {
        ComponentToRemove->Destroy();
        Entity->Components.erase(ComponentToRemove->Type);

        return true;
    }

    std::unordered_map<EntityId, Entity>& GetEntities() {
        return Entities;
    }

private:
    std::unordered_map<EntityId, Entity> Entities;
};
