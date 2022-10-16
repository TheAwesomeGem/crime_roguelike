#include "UpdateManager.h"
#include "CommandProcessor.h"
#include "ZoneManager.h"


void UpdateManager::Update(GameState* Game) {
    // TODO: Which order should we process these commands?

    for (auto& [_, Controller]: Game->GetAllControllers()) {
        Entity* ControlledEntity = Game->CurrentWorld.GetEntity(Controller.ControlledEntityId);
        CommandProcessor::ProcessCommand(&Controller);
        Point OldZoneCoordinate = ControlledEntity->GetOldCoord();
        Point NewZoneCoordinate = ControlledEntity->GetCoord();

        if (OldZoneCoordinate != NewZoneCoordinate) {
            printf("CHANGE OF ZONE DETECTED!\n");

            ControlledEntity->SetCoordCurrent();
            ZoneManager::ChangedZone(Game, ControlledEntity, OldZoneCoordinate, NewZoneCoordinate);
        }
    }
}