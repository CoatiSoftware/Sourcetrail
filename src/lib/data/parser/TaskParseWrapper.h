#ifndef TASK_PARSE_WRAPPER_H
#define TASK_PARSE_WRAPPER_H

#include <memory>

#include "data/parser/Parser.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/scheduling/Task.h"
#include "utility/scheduling/TaskRunner.h"
#include "utility/scheduling/TaskDecorator.h"
#include "utility/TimePoint.h"

class DialogView;
class FileRegister;
class PersistentStorage;

class TaskParseWrapper
	: public TaskDecorator
{
public:
	TaskParseWrapper(
		PersistentStorage* storage,
		std::shared_ptr<DialogView> dialogView
	);
	virtual ~TaskParseWrapper();

	virtual void setTask(std::shared_ptr<Task> task);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	PersistentStorage* m_storage;
	std::shared_ptr<DialogView> m_dialogView;

	TimePoint m_start;
	std::shared_ptr<TaskRunner> m_taskRunner;
};

#endif // TASK_PARSE_WRAPPER_H
