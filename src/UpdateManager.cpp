#include "UpdateManager.h"
#include "ZoneManager.h"
#include "CommandProcessor.h"


void UpdateManager::UpdateZone(World* CurrentWorld, Point ZoneCoordinate) {
    Zone* ZoneToUpdate = ZoneManager::GetZoneFromCoord(CurrentWorld, ZoneCoordinate);

    if (ZoneToUpdate == nullptr) {
        printf("HUGE PROBLEM HERE FOLKS!\n");

        return;
    }

    // TODO: This is not good. We need to figure out how to destroy a zone when no one is there. Should we really loop through all the entities or have a temporary cache?

    if (ZoneToUpdate->TurnsSinceLastCreated == -1 || ZoneToUpdate->TurnsSinceLastCreated >= Zone::TURNS_TO_REGENERATE) {
        // TODO: Regenerate zone along with each entity and it's AIController.
    }

    // TODO: Updates each component that needs to be updated
}

void UpdateManager::Update(World* CurrentWorld, Controller* Controller) {
    CommandProcessor::ProcessCommand(Controller);

    // TODO: This should be fine since NPC only gets created/destroyed on Playable Character's zone. But still check
    for (auto& [_, ActiveEntity]: CurrentWorld->Entities.GetEntities()) {
        UpdateZone(CurrentWorld, ActiveEntity.ZoneCoordinate);
    }
}