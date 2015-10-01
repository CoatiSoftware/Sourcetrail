#include "data/TaskCleanStorage.h"

#include "data/Storage.h"
#include "utility/messaging/type/MessageStatus.h"

TaskCleanStorage::TaskCleanStorage(Storage* storage, const std::set<FilePath>& filePaths)
	: m_storage(storage)
	, m_fileCount(filePaths.size())
{
	for (const FilePath& p : filePaths)
	{
		m_filePaths.push(p);
	}
}

void TaskCleanStorage::enter()
{
	m_start = utility::durationStart();
}

Task::TaskState TaskCleanStorage::update()
{
	if (!m_filePaths.size())
	{
		if (m_fileCount)
		{
			MessageStatus("Cleaning up names", false, true).dispatch();
			m_storage->removeUnusedNames();
		}

		return Task::STATE_FINISHED;
	}

	FilePath filePath = m_filePaths.front();
	m_filePaths.pop();

	std::stringstream ss;
	ss << "clearing (ESC to quit): [";
	ss << m_fileCount - m_filePaths.size() - 1 << "/" << m_fileCount << "] ";
	ss << filePath.str();

	MessageStatus(ss.str(), false, true).dispatch();

	m_storage->clearFileElement(filePath);

	return Task::STATE_RUNNING;
}

void TaskCleanStorage::exit()
{
	// std::cout << "clean duration: " << utility::duration(m_start) << std::endl;
}

void TaskCleanStorage::interrupt()
{
}

void TaskCleanStorage::revert()
{
}
