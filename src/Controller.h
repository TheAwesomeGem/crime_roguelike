#pragma once

#include "Common.h"
#include "Entity.h"
#include "Command.h"


class Controller {
public:
    explicit Controller(ControllerId Id_);

    void Possess(Entity* EntityToControl);

    void AddCommand(std::unique_ptr<Command> CommandToAdd);

    void PopCommand();

    Command* GetCommand() const {
        return Commands.front().get();
    }

    bool HasCommands() const {
        return !Commands.empty();
    }

public:
    ControllerId Id;
    EntityId ControlledEntityId;

private:
    std::deque<std::unique_ptr<Command>> Commands;
};
