#include "data/parser/java/TaskParseJava.h"

TaskParseJava::TaskParseJava(
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
{
}

void TaskParseJava::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskParseJava::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	return STATE_SUCCESS;
}

void TaskParseJava::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParseJava::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParseJava::handleMessage(MessageInterruptTasks* message)
{
}
