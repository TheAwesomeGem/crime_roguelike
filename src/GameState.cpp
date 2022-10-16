#include "GameState.h"


GameState::GameState() : Controllers{}, PlayerControllerId{}, CurrentWorld{} {
    Init();
}

GameState::~GameState() {
    Deinit();
}

void GameState::Init() {
    CurrentWorld.EntityDestroyEvent.AddRaw(this, &GameState::OnEntityDestroyed);
}

void GameState::Deinit() {

}

void GameState::OnEntityDestroyed(Entity* DestroyedEntity, ControllerId ControllerIdToDestroy) {
    Controller* ControllerToDestroy = GetController(ControllerIdToDestroy);

    if (ControllerToDestroy->Id != PlayerControllerId) {
        RemoveController(ControllerToDestroy->Id);
    }
}

// First controller is always Player's controller
Controller* GameState::CreateController() {
    ControllerId NewId = Random::Uuid();
    auto ControllerIt = Controllers.emplace(NewId, NewId);

    if (PlayerControllerId.is_nil()) {
        PlayerControllerId = NewId;
    }

    return &ControllerIt.first->second;
}

void GameState::RemoveController(ControllerId IdToRemove) {
    Controllers.erase(IdToRemove);
}

Controller* GameState::GetController(ControllerId Id) {
    auto ControllerIt = Controllers.find(Id);

    if (ControllerIt == Controllers.end()) {
        return nullptr;
    }

    return &ControllerIt->second;
}

Controller* GameState::GetPlayerController() {
    if (PlayerControllerId.is_nil()) {
        return nullptr;
    }

    return GetController(PlayerControllerId);
}

std::pair<Controller*, Entity*> GameState::CreateEntityWithController(Point ZoneCoordinate) {
    Controller* NewController = CreateController();
    Entity* SpawnedEntity = CurrentWorld.SpawnEntity();
    NewController->Possess(SpawnedEntity);
    SpawnedEntity->SetCoord(ZoneCoordinate);

    return std::make_pair(NewController, SpawnedEntity);
}
