#include "Controller.h"


Controller::Controller(ControllerId Id_) : Id{Id_}, ControlledEntityId{}, Commands{} {

}

void Controller::Possess(Entity* EntityToControl) {
    ControlledEntityId = EntityToControl->Id;
    EntityToControl->ControlledBy = Id;
}

void Controller::AddCommand(std::unique_ptr<Command> CommandToAdd) {
    Commands.push_back(std::move(CommandToAdd));
}

void Controller::PopCommand() {
    Commands.pop_front();
}
