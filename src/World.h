#pragma once

#include "Common.h"
#include "Entity.h"
#include "Component.h"
#include "EntityStorage.h"
#include "ComponentStorage.h"
#include "Zone.h"


class World {
public:
    MulticastDelegate<Entity*, ControllerId> EntityDestroyEvent;

    static constexpr const uint16_t ZONES_ROWS = 5;
    static constexpr const uint16_t ZONES_COLS = 5;
    static constexpr const uint16_t ZONES_DEPTH = 3;

public:
    explicit World();

    Entity* SpawnEntity(Point ZoneCoordinate);

    void DestroyEntity(Entity* EntityToDestroy);

    Entity* GetEntity(EntityId IdToGet);

    template<typename T>
    T* CreateComponent(Entity* EntityToAttachTo) {
        T* NewComponent = Components.CreateComponent<T>(Random::Uuid());
        EntityStorage::AttachComponent(EntityToAttachTo, NewComponent);

        return NewComponent;
    }

public:
    ComponentStorage Components;
    EntityStorage Entities;
    std::unordered_map<Point, Zone> Zones;

private:
    void GenerateEmptyZones();
};
