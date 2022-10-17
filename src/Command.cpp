#include "Command.h"


Command::Command(World* World, Entity* Entity) : CurrentWorld{World}, CurrentEntity{Entity} {

}


MovementCommand::MovementCommand(World* World, Entity* Entity, Point Coordinate) : Command(World, Entity), ToCoordinate{Coordinate} {

}

void MovementCommand::Execute() {
    Point CurrentZoneCoord = CurrentEntity->GetCoord();
    CurrentZoneCoord += ToCoordinate;
    CurrentEntity->SetCoord(CurrentZoneCoord);


    printf("Executed Move Command to [%d,%d,%d]\n", ToCoordinate.X, ToCoordinate.Y, ToCoordinate.Z);
}

int MovementCommand::TurnAmount() {
    return 3;
}
