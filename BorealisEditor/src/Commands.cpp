#include <BorealisPCH.hpp>
#include <Commands.hpp>

namespace Borealis
{
	std::stack<std::unique_ptr<ICommand>> ActionManager::undoStack;
	std::stack<std::unique_ptr<ICommand>> ActionManager::redoStack;

	void ActionManager::execute(std::unique_ptr<ICommand> command)
	{
		command->execute();
		undoStack.push(std::move(command));
		while (!redoStack.empty()) redoStack.pop();
	}
    void ActionManager::undo() {
        if (!undoStack.empty()) {
            auto command = std::move(undoStack.top());
            undoStack.pop();
            command->undo();
            redoStack.push(std::move(command));
        }
    }

    void ActionManager::redo() {
        if (!redoStack.empty()) {
            auto command = std::move(redoStack.top());
            redoStack.pop();
            command->execute();
            undoStack.push(std::move(command));
        }
    }
}
