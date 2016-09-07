#include "data/TaskInjectStorage.h"

#include "data/Storage.h"

TaskInjectStorage::TaskInjectStorage(
	std::shared_ptr<Storage> source,
	std::shared_ptr<Storage> target
)
	: m_source(source)
	, m_target(target)
{
}

void TaskInjectStorage::doEnter()
{
}

Task::TaskState TaskInjectStorage::doUpdate()
{
	m_target->inject(m_source.get());
	return STATE_SUCCESS;
}

void TaskInjectStorage::doExit()
{
}

void TaskInjectStorage::doReset()
{
}
