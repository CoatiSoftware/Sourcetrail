#include "TaskMergeStorages.h"

#include <chrono>
#include <thread>

#include "StorageProvider.h"

TaskMergeStorages::TaskMergeStorages(
	std::shared_ptr<StorageProvider> storageProvider
)
	: m_storageProvider(storageProvider)
{
}

void TaskMergeStorages::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskMergeStorages::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_storageProvider->getStorageCount() > 2) // largest storage won't be touched here
	{
		std::shared_ptr<IntermediateStorage> target = m_storageProvider->consumeSecondLargestStorage();
		std::shared_ptr<IntermediateStorage> source = m_storageProvider->consumeSecondLargestStorage();
		if (target && source)
		{
			target->inject(source.get());
			m_storageProvider->insert(target);
			return STATE_SUCCESS;
		}
		else
		{
			if (target)
			{
				m_storageProvider->insert(target);
			}
			if (source)
			{
				m_storageProvider->insert(source);
			}
		}
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	return STATE_FAILURE;
}

void TaskMergeStorages::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskMergeStorages::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
