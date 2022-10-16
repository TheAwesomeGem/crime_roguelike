#pragma once

#include "Common.h"
#include "World.h"
#include "Controller.h"


namespace UpdateManager {
    void UpdateZone(World* CurrentWorld, Point ZoneCoordinate);

    void Update(World* CurrentWorld, Controller* Controller);
}
