#pragma once

#include "Common.h"
#include "Entity.h"
#include "World.h"


struct Command {
    explicit Command(World* World, Entity* Entity);

    virtual ~Command() = default;

    virtual void Execute() = 0;

    virtual int TurnAmount() = 0;

    Zone* GetEntityZone() {
        return CurrentWorld->GetZone(CurrentEntity->GetCoord());
    }

    Zone* GetOldEntityZone() {
        return CurrentWorld->GetZone(CurrentEntity->GetOldCoord());
    }

    World* CurrentWorld;
    Entity* CurrentEntity;
};

struct MovementCommand : Command {
    explicit MovementCommand(World* World, Entity* Entity, Point Coordinate);

    virtual void Execute() override;

    int TurnAmount() override;

private:
    Point ToCoordinate;
};
