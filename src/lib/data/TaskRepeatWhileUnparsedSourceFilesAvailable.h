#ifndef TASK_REPEAT_WHILE_UNPARSED_SOURCE_FILES_AVAILABLE_H
#define TASK_REPEAT_WHILE_UNPARSED_SOURCE_FILES_AVAILABLE_H

#include <vector>

#include "utility/scheduling/TaskDecorator.h"
#include "utility/scheduling/TaskRunner.h"

class FileRegister;

class TaskRepeatWhileUnparsedSourceFilesAvailable
	: public TaskDecorator
{
public:
	TaskRepeatWhileUnparsedSourceFilesAvailable(
		std::shared_ptr<FileRegister> fileRegister
	);

	virtual void setTask(std::shared_ptr<Task> task);

private:
	virtual void doEnter();
	virtual TaskState doUpdate();
	virtual void doExit();
	virtual void doReset();

	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<TaskRunner> m_taskRunner;
};

#endif // TASK_REPEAT_WHILE_UNPARSED_SOURCE_FILES_AVAILABLE_H
