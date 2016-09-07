#include "data/parser/java/TaskParseJava.h"

TaskParseJava::TaskParseJava(
	std::shared_ptr<IntermediateStorage> storage,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
{
}

void TaskParseJava::doEnter()
{
}

Task::TaskState TaskParseJava::doUpdate()
{
	return STATE_SUCCESS;
}

void TaskParseJava::doExit()
{
}

void TaskParseJava::doReset()
{
}

void TaskParseJava::handleMessage(MessageInterruptTasks* message)
{
}
