#pragma once

#include "Common.h"
#include "Component.h"


struct Entity {
    static constexpr const int MaxTurnAmount = 3;

    explicit Entity(EntityId Identifier);

    template<typename T>
    T* GetComponent(ComponentType Type) {
        auto ComponentIt = Components.find(Type);

        if (ComponentIt == Components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(ComponentIt->second);
    }

    void SetCoord(Point NewZoneCoordinate);

    void SetCoordCurrent();

    Point GetCoord() const {
        return ZoneCoordinate;
    }

    Point GetOldCoord() const {
        return OldZoneCoordinate;
    }

public:
    EntityId Id;
    std::unordered_map<ComponentType, BaseComponent*> Components;
    ControllerId ControlledBy;
    int TurnAmountLeft;

private:
    Point ZoneCoordinate;
    Point OldZoneCoordinate;
};
