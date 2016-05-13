#ifndef TASK_PARSE_WRAPPER_H
#define TASK_PARSE_WRAPPER_H

#include <memory>

#include "data/parser/Parser.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class PersistentStorage;
class FileRegister;
class CxxParser;

class TaskParseWrapper
	: public Task
{
public:

	TaskParseWrapper(
		std::shared_ptr<Task> child,
		PersistentStorage* storage,
		std::shared_ptr<FileRegister> fileRegister
	);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	std::shared_ptr<Task> m_child;
	PersistentStorage* m_storage;
	std::shared_ptr<FileRegister> m_fileRegister;

	TimePoint m_start;
};

#endif // TASK_PARSE_WRAPPER_H
