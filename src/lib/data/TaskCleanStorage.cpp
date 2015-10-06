#include "data/TaskCleanStorage.h"

#include "data/Storage.h"
#include "utility/messaging/type/MessageStatus.h"

TaskCleanStorage::TaskCleanStorage(Storage* storage, const std::vector<FilePath>& filePaths)
	: m_storage(storage)
	, m_filePaths(filePaths)
	, m_fileCount(filePaths.size())
{
}

void TaskCleanStorage::enter()
{
	m_start = utility::durationStart();
}

Task::TaskState TaskCleanStorage::update()
{
	if (m_filePaths.size())
	{
		std::stringstream ss;
		ss << "clearing " << m_filePaths.size() << " files (ESC to quit)";
		MessageStatus(ss.str(), false, true).dispatch();

		m_storage->clearFileElements(m_filePaths);

		m_filePaths.clear();

		return Task::STATE_RUNNING;
	}

	if (m_fileCount)
	{
		MessageStatus("Cleaning up names (ESC to quit)", false, true).dispatch();
		m_storage->removeUnusedNames();
	}

	return Task::STATE_FINISHED;
}

void TaskCleanStorage::exit()
{
	std::stringstream ss;
	ss << "clearing files done, ";
	ss << std::setprecision(2) << std::fixed << utility::duration(m_start) << " seconds";
	MessageStatus(ss.str()).dispatch();
}

void TaskCleanStorage::interrupt()
{
	MessageStatus("clearing files interrupted", false, true).dispatch();
}

void TaskCleanStorage::revert()
{
}
