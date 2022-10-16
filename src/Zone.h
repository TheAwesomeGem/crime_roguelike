#pragma once

#include "Common.h"


enum class ZoneComponentType {
    Police
};

struct ZoneComponent {
    explicit ZoneComponent(ZoneComponentType ComponentType)
            : Type{ComponentType} {

    }

    ZoneComponentType Type;
};

struct Zone {
    static inline const Point InvalidZoneCoord{-1, -1, -1};

    explicit Zone(Point ZoneCoordinate);

    template<typename T>
    void AddComponent(ZoneComponent* ComponentToAdd) {
        if (Components.find(ComponentToAdd->Type) != Components.end()) {
            printf("Component already exists.\n");
            return;
        }

        Components.emplace(ComponentToAdd->Type, std::unique_ptr<ZoneComponent>{ComponentToAdd});
    }

    template<typename T>
    void RemoveComponent(ZoneComponentType ComponentTypeToRemove) {
        if (Components.find(ComponentTypeToRemove) == Components.end()) {
            printf("Component does not exists already.\n");
            return;
        }

        Components.erase(ComponentTypeToRemove);
    }

    template<typename T>
    T* GetComponent(ZoneComponentType Type) {
        auto ComponentIt = Components.find(Type);

        if (ComponentIt == Components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(ComponentIt->second.get());
    }

    bool IsGenerated() const {
        return !NPCEntities.empty();
    }

    bool ShouldDestroy() const {
        return PlayableEntitiesCache.empty();
    }

    Point Coordinate; // Zone-Space Coordinate. For example Zone A (0, 0) and Zone B(1, 0)
    std::vector<EntityId> NPCEntities; // TODO: Should group them between civilian and police
    std::unordered_set<EntityId> PlayableEntitiesCache;

private:
    std::unordered_map<ZoneComponentType, std::unique_ptr<ZoneComponent>> Components; // All the components that extend the state of the Zone.
};
