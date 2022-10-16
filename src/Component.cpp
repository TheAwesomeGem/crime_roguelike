#include "Component.h"


BaseComponent::BaseComponent(ComponentId Identifier, ComponentType ComponentType)
        : Id{Identifier}, Type{ComponentType} {

}

void BaseComponent::Destroy() {
    DestroyEvent.Broadcast(Id);
}


HealthComponent::HealthComponent(ComponentId Identifier)
        : BaseComponent{Identifier, ComponentType::Health}, CurrentHealth{0.0F}, MaxHealth{0.0F} {

}

void HealthComponent::SetHealth(float Health) {
    float NewHealth = CLAMP(Health, -std::numeric_limits<float>::max(), MaxHealth);

    CurrentHealth = NewHealth;

    if (NewHealth <= 0.0F) {
        DeathEvent.Broadcast();
    }
}

void HealthComponent::SetMaxHealth(float Health) {
    float NewMaxHealth = CLAMP(Health, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    MaxHealth = NewMaxHealth;
}