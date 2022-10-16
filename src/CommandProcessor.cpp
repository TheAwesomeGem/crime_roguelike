#include "CommandProcessor.h"


void CommandProcessor::ProcessCommand(Controller* Controller) {
    while (Controller->HasCommands()) {
        Controller->GetCommand()->Execute();
        Controller->PopCommand();
    }
}
