#include "ComponentStorage.h"


ComponentStorage::ComponentStorage()
        : Components{} {

}

bool ComponentStorage::Destroy(ComponentId IdToDestroy) {
    auto ComponentIt = Components.find(IdToDestroy);

    if (ComponentIt != Components.end()) {
        BaseComponent* ComponentToDestroy = ComponentIt->second;
        ComponentToDestroy->Id = uuids::uuid{};
        delete ComponentToDestroy;
        Components.erase(ComponentIt);

        return true;
    }

    return false;
}

void ComponentStorage::AddComponent(BaseComponent* ComponentToAdd) {
    auto ComponentIt = Components.emplace(ComponentToAdd->Id, ComponentToAdd);
    ComponentToAdd->DestroyEvent.AddRaw(this, &ComponentStorage::OnDestroy);
}

void ComponentStorage::OnDestroy(ComponentId Id) {
    Destroy(Id);
}
