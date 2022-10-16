#pragma once

#include "Common.h"
#include "Component.h"


class ComponentStorage {
public:
    ComponentStorage();

    template<typename T>
    T* CreateComponent(ComponentId IdToAdd) {
        T* NewComponent = new T{IdToAdd};
        AddComponent(NewComponent);

        return NewComponent;
    }

    bool Destroy(ComponentId IdToDestroy);

private:
    void AddComponent(BaseComponent* ComponentToAdd);

    void OnDestroy(ComponentId Id);

private:
    std::unordered_map<ComponentId, BaseComponent*> Components;
};

