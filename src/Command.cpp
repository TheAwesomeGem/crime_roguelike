#include "Command.h"


Command::Command(World* World, EntityId Id) : CurrentWorld{World}, CurrentEntityId{Id} {

}


MovementCommand::MovementCommand(World* World, EntityId Id, Point Coordinate) : Command(World, Id), ToCoordinate{Coordinate} {

}

void MovementCommand::Execute() {
    printf("Executed Move Command to [%d,%d,%d]\n", ToCoordinate.X, ToCoordinate.Y, ToCoordinate.Z);
}
