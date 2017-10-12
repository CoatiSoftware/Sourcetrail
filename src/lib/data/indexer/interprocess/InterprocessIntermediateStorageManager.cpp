#include "InterprocessIntermediateStorageManager.h"

#include "data/indexer/interprocess/shared_types/SharedIntermediateStorage.h"
#include "data/storage/IntermediateStorage.h"
#include "utility/logging/logging.h"

const char* InterprocessIntermediateStorageManager::s_sharedMemoryNamePrefix = "iist_";

const char* InterprocessIntermediateStorageManager::s_intermediatStoragesKeyName = "intermediate_storages";

InterprocessIntermediateStorageManager::InterprocessIntermediateStorageManager(
	const std::string& instanceUuid, Id processId, bool isOwner
)
	: BaseInterprocessDataManager(
		s_sharedMemoryNamePrefix + std::to_string(processId) + "_" + instanceUuid,
        3 * 1048576 /* 3 MB */,
		instanceUuid,
		processId,
		isOwner)
{
}

InterprocessIntermediateStorageManager::~InterprocessIntermediateStorageManager()
{
}

void InterprocessIntermediateStorageManager::pushIntermediateStorage(
	const std::shared_ptr<IntermediateStorage>& intermediateStorage)
{
    const unsigned int overestimationMultiplier = 3;
    size_t size = (intermediateStorage->getByteSize() + sizeof(SharedIntermediateStorage)) * overestimationMultiplier + 1048576/* 1 MB */;

	SharedMemory::ScopedAccess access(&m_sharedMemory);

	size_t freeMemory = access.getFreeMemorySize();
	if (freeMemory < size)
	{
		LOG_INFO_STREAM(
			<< "grow memory - est: " << size << " size: " << access.getMemorySize()
			<< " free: " << access.getFreeMemorySize() << " alloc: " << (size - freeMemory));

		access.growMemory(size - freeMemory);

		LOG_INFO("growing memory succeeded");
	}

	SharedMemory::Queue<SharedIntermediateStorage>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(s_intermediatStoragesKeyName);
	if (!queue)
	{
		return;
	}

	queue->push_back(SharedIntermediateStorage(access.getAllocator()));
	SharedIntermediateStorage& storage = queue->back();

	storage.setStorageNodes(intermediateStorage->getStorageNodes());
	storage.setStorageFiles(intermediateStorage->getStorageFiles());
	storage.setStorageSymbols(intermediateStorage->getStorageSymbols());
	storage.setStorageEdges(intermediateStorage->getStorageEdges());
	storage.setStorageLocalSymbols(intermediateStorage->getStorageLocalSymbols());
	storage.setStorageSourceLocations(intermediateStorage->getStorageSourceLocations());
	storage.setStorageOccurrences(intermediateStorage->getStorageOccurrences());
	storage.setStorageComponentAccesses(intermediateStorage->getComponentAccesses());
	storage.setStorageCommentLocations(intermediateStorage->getCommentLocations());
	storage.setStorageErrors(intermediateStorage->getErrors());

	storage.setNextId(intermediateStorage->getNextId());

	LOG_INFO(access.logString());
}

std::shared_ptr<IntermediateStorage> InterprocessIntermediateStorageManager::popIntermediateStorage()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIntermediateStorage>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(s_intermediatStoragesKeyName);
	if (!queue || !queue->size())
	{
		return nullptr;
	}

	SharedIntermediateStorage& sharedIntermediateStorage = queue->front();

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

	storage->setStorageNodes(sharedIntermediateStorage.getStorageNodes());
	storage->setStorageFiles(sharedIntermediateStorage.getStorageFiles());
	storage->setStorageSymbols(sharedIntermediateStorage.getStorageSymbols());
	storage->setStorageEdges(sharedIntermediateStorage.getStorageEdges());
	storage->setStorageLocalSymbols(sharedIntermediateStorage.getStorageLocalSymbols());
	storage->setStorageSourceLocations(sharedIntermediateStorage.getStorageSourceLocations());
	storage->setStorageOccurrences(sharedIntermediateStorage.getStorageOccurrences());
	storage->setComponentAccesses(sharedIntermediateStorage.getStorageComponentAccesses());
	storage->setCommentLocations(sharedIntermediateStorage.getStorageCommentLocations());
	storage->setErrors(sharedIntermediateStorage.getStorageErrors());

	storage->setNextId(sharedIntermediateStorage.getNextId());

	queue->pop_front();
    LOG_INFO(access.logString());

	return storage;
}

size_t InterprocessIntermediateStorageManager::getIntermediateStorageCount()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIntermediateStorage>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(s_intermediatStoragesKeyName);
	if (!queue)
	{
		return 0;
	}

	return queue->size();
}
