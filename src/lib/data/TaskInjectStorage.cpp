#include "TaskInjectStorage.h"

#include "Storage.h"
#include "StorageProvider.h"

TaskInjectStorage::TaskInjectStorage(
	std::shared_ptr<StorageProvider> storageProvider,
	std::weak_ptr<Storage> target
)
	: m_storageProvider(storageProvider)
	, m_target(target)
{
}

void TaskInjectStorage::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskInjectStorage::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_storageProvider->getStorageCount() > 0)
	{
		std::shared_ptr<IntermediateStorage> source = m_storageProvider->consumeLargestStorage();
		if (source)
		{
			if (std::shared_ptr<Storage> target = m_target.lock())
			{
				target->inject(source.get());
				return STATE_SUCCESS;
			}
		}
	}

	return STATE_FAILURE;
}

void TaskInjectStorage::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskInjectStorage::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskInjectStorage::handleMessage(MessageIndexingInterrupted* message)
{
	m_storageProvider->clear();
}
