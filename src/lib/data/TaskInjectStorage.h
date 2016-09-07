#ifndef TASK_INJECT_STORAGE_H
#define TASK_INJECT_STORAGE_H

#include <vector>

#include "utility/scheduling/Task.h"

class Storage;

class TaskInjectStorage
	: public Task
{
public:
	TaskInjectStorage(
		std::shared_ptr<Storage> source,
		std::shared_ptr<Storage> target
	);

private:
	virtual void doEnter();
	virtual TaskState doUpdate();
	virtual void doExit();
	virtual void doReset();

	std::shared_ptr<Storage> m_source;
	std::shared_ptr<Storage> m_target;
};

#endif // TASK_INJECT_STORAGE_H
