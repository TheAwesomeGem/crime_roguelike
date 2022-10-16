#include <iostream>
#include "Common.h"
#include "GameState.h"
#include "UpdateManager.h"


struct EventData {
    EventData()
            : ZoneCoordinate{}, Instigator{nullptr}, Target{nullptr} {

    }

    Point ZoneCoordinate;
    // EventType Type;
    Entity* Instigator;
    Entity* Target;
};

struct History {
    std::vector<EventData> Events;
};


std::ostream& operator<<(std::ostream& stream, Vec3 Location) {
    stream << "[" << Location.X << ", " << Location.Y << "]";

    return stream;
}

std::ostream& operator<<(std::ostream& stream, Point Coordinate) {
    stream << "(" << Coordinate.X << ", " << Coordinate.Y << ")";

    return stream;
}

namespace MovementInput {
    static void DoMovement(Controller* InputController, std::string_view Input, World* CurrentWorld) {
        InputController->AddCommand(std::make_unique<MovementCommand>(CurrentWorld, InputController->ControlledEntityId, Point{1, 0, 0}));
    }
}

int main() {
    GameState Game;
    Controller* PlayerController = Game.CreateController();
    Entity* PlayerEntity = Game.CurrentWorld.SpawnEntity(Point{0, 0, 0});
    PlayerController->Possess(PlayerEntity);


    // TODO: Add turn system and for now, it will be only Player. Add all the commands a Player makes before they finish the turn and process it in an update loop to
    // TODO: generate states for the next turn. Rinse and repeat.

    // TODO: Maybe instead of using events, anytime an entity moves to a zone, we generate state to that zone. For example, we will generate other entities randomly.
    // TODO: Whenever an entity leaves the zone, the state gets cleared. We will need a distinction between player/AI controlled entities and generated NPC entities.
    // TODO: NPC entities should not be allowed to travel between zones and they are stuck on the zone they are generated in and gets destroyed once playable entity leaves the zone.
    // TODO: Now after the temporary state of a zone gets generated, player can assign commands to the playable entity based on the state of the zone.
    // TODO: There will be permanent state of the zone and that can be stored inside a component. Things such as police population and civilian population and the type of zone.

    // TODO: ControlledEntities = Playable Entities
    // TODO: !ControlledEntities = NPC Entities

    UpdateManager::Update(&Game.CurrentWorld, PlayerController); // Initial Update to Regenerate all the zones that a Playable Entity is at.

    std::string Input;
    do {
        std::cout << "Type your input: \n";
        std::getline(std::cin, Input);

        if (Input.empty()) {
            continue;
        }

        std::transform(Input.begin(), Input.end(), Input.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (Input == "forward" || Input == "back" || Input == "left" || Input == "right") {
            MovementInput::DoMovement(PlayerController, Input, &Game.CurrentWorld);
        } else if (Input == "end") {
            break;
            // TODO: Player has ended the turn, let TurnManager dictate who takes their turn next.
        } else {
            printf("Unknown Command.\n");
        }
    }

    while (!Input.empty());

    // TODO: LOOP THROUGH EACH AI CONTROLLER HERE AND GENERATE COMMANDS

    UpdateManager::Update(&Game.CurrentWorld, PlayerController); // TODO: This will be a list of Controllers after adding AI.

    return 0;
}
