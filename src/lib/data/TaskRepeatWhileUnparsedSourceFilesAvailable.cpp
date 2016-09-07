#include "data/TaskRepeatWhileUnparsedSourceFilesAvailable.h"

#include "utility/file/FileRegister.h"

TaskRepeatWhileUnparsedSourceFilesAvailable::TaskRepeatWhileUnparsedSourceFilesAvailable(
	std::shared_ptr<FileRegister> fileRegister
)
	: m_fileRegister(fileRegister)
{
}

void TaskRepeatWhileUnparsedSourceFilesAvailable::setTask(std::shared_ptr<Task> task)
{
	if (task)
	{
		m_taskRunner = std::make_shared<TaskRunner>(task);
	}
}

void TaskRepeatWhileUnparsedSourceFilesAvailable::doEnter()
{
}

Task::TaskState TaskRepeatWhileUnparsedSourceFilesAvailable::doUpdate()
{
	TaskState state = m_taskRunner->update();

	if (state == Task::STATE_SUCCESS)
	{
		if(m_fileRegister->getUnparsedSourceFilePaths().size() != 0)
		{
			state = Task::STATE_RUNNING;
			m_taskRunner->reset();
		}
	}

	return state;
}

void TaskRepeatWhileUnparsedSourceFilesAvailable::doExit()
{
}

void TaskRepeatWhileUnparsedSourceFilesAvailable::doReset()
{
	m_taskRunner->reset();
}
