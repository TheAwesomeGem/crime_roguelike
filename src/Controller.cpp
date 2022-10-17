#include "Controller.h"


Controller::Controller(ControllerId Id_) : Id{Id_}, ControlledEntityId{}, Commands{} {

}

void Controller::Possess(Entity* EntityToControl) {
    ControlledEntityId = EntityToControl->Id;
    EntityToControl->ControlledBy = Id;
}

bool Controller::AddCommand(std::unique_ptr<Command> CommandToAdd) {
    Entity* CommandEntity = CommandToAdd->CurrentEntity;
    int TurnAmount = CommandToAdd->TurnAmount();

    if (CommandEntity->TurnAmountLeft < TurnAmount) {
        return false;
    }

    CommandEntity->TurnAmountLeft -= TurnAmount;

    Commands.push_back(std::move(CommandToAdd));

    return true;
}

void Controller::PopCommand() {
    Command* CommandToPop = Commands.front().get();
    Entity* CommandEntity = CommandToPop->CurrentEntity;
    CommandEntity->TurnAmountLeft += CommandToPop->TurnAmount();

    Commands.pop_front();
}
