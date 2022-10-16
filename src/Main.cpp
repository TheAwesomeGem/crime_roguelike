#include <iostream>
#include "Common.h"
#include "GameState.h"
#include "UpdateManager.h"


// TODO: Main:
// TODO: Add basic commands and restriction for 1 command per entity

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
        InputController->AddCommand(
                std::make_unique<MovementCommand>(
                        CurrentWorld,
                        CurrentWorld->GetEntity(InputController->ControlledEntityId),
                        Point{1, 0, 0})
        );
    }
}

int main() {
    GameState Game;
    auto [PlayerController, _] = Game.CreateEntityWithController(Point{0, 0, 0});

    // TODO: There will be permanent state of the zone and that can be stored inside a component. Things such as police population and civilian population and the type of zone.

    UpdateManager::Update(&Game); // Initial Update to Regenerate all the zones that a Playable Entity is at.

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
            // TODO: Add AI Commands now to simulate AI's turn.
            // TODO: Player has ended the turn, let TurnManager dictate who takes their turn next.
            UpdateManager::Update(&Game);
        } else {
            printf("Unknown Command.\n");
        }
    }

    while (!Input.empty());

    return 0;
}
