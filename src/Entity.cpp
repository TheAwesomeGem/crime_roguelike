#include "Entity.h"
#include "Zone.h"


Entity::Entity(EntityId Identifier)
        : Id{Identifier}, Components{}, ControlledBy{}, ZoneCoordinate{Zone::InvalidZoneCoord}, OldZoneCoordinate{ZoneCoordinate}, TurnAmountLeft{MaxTurnAmount} {

}

void Entity::SetCoord(Point NewZoneCoordinate) {
    if (NewZoneCoordinate == ZoneCoordinate) {
        return;
    }

    OldZoneCoordinate = ZoneCoordinate;
    ZoneCoordinate = NewZoneCoordinate;
}

void Entity::SetCoordCurrent() {
    OldZoneCoordinate = ZoneCoordinate;
}
