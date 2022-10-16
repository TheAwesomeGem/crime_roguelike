#pragma once

#include "Common.h"
#include "Component.h"


struct Entity {
    explicit Entity(EntityId Identifier);

    template<typename T>
    T* GetComponent(ComponentType Type) {
        auto ComponentIt = Components.find(Type);

        if (ComponentIt == Components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(ComponentIt->second);
    }

public:
    EntityId Id;
    std::unordered_map<ComponentType, BaseComponent*> Components;
    Point ZoneCoordinate;
    ControllerId ControlledBy;
};
