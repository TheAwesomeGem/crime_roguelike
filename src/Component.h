#pragma once

#include "Common.h"


enum class ComponentType {
    Health
};

struct BaseComponent {
    MulticastDelegate<ComponentId> DestroyEvent;

    BaseComponent(ComponentId Identifier, ComponentType ComponentType);

    virtual void Destroy();

    ComponentId Id;
    ComponentType Type;
};

struct HealthComponent : BaseComponent {
    MulticastDelegate<> DeathEvent;

    explicit HealthComponent(ComponentId Identifier);


    bool IsAlive() const {
        return CurrentHealth > 0.0F;
    }

    bool IsDead() const {
        return !IsAlive();
    }

    float Health() const {
        return CurrentHealth;
    }

    float HealthMax() const {
        return MaxHealth;
    }

    void SetHealth(float Health);

    void SetMaxHealth(float Health);

private:
    float CurrentHealth;
    float MaxHealth;
};

