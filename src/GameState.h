#pragma once

#include "Common.h"
#include "World.h"
#include "Controller.h"


struct GameState {
    GameState();

    ~GameState();

    void Init();

    void Deinit();

    void OnEntityDestroyed(Entity* DestroyedEntity, ControllerId ControllerIdToDestroy);

    // First controller is always Player's controller
    Controller* CreateController();

    void RemoveController(ControllerId IdToRemove);

    Controller* GetController(ControllerId Id);

    Controller* GetPlayerController();

    std::pair<Controller*, Entity*> CreateEntityWithController(Point ZoneCoordinate);

    std::unordered_map<ControllerId, Controller>& GetAllControllers() {
        return Controllers;
    }

private:
    std::unordered_map<ControllerId, Controller> Controllers;
    ControllerId PlayerControllerId;

public:
    World CurrentWorld;
};
