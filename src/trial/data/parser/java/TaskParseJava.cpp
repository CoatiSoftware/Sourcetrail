#include "data/parser/java/TaskParseJava.h"

TaskParseJava::TaskParseJava(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments
)
{
}

void TaskParseJava::enter()
{
}

Task::TaskState TaskParseJava::update()
{
	return Task::STATE_FINISHED;
}

void TaskParseJava::exit()
{
}

void TaskParseJava::interrupt()
{
}

void TaskParseJava::revert()
{
}
