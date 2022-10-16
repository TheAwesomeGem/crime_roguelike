#pragma once

#include "Common.h"
#include "Entity.h"
#include "World.h"


struct Command {
    explicit Command(World* World, EntityId Id);

    virtual ~Command() = default;

    virtual void Execute() = 0;

    Entity* GetEntity() {
        return CurrentWorld->GetEntity(CurrentEntityId);
    }

    World* CurrentWorld;

private:
    EntityId CurrentEntityId;
};

struct MovementCommand : Command {
    explicit MovementCommand(World* World, EntityId Id, Point Coordinate);

    virtual void Execute() override;

private:
    Point ToCoordinate;
};
