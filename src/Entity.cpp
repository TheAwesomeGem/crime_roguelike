#include "Entity.h"


Entity::Entity(EntityId Identifier)
        : Id{Identifier}, Components{}, ZoneCoordinate{}, ControlledBy{} {

}
