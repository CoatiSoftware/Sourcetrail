#include "InterprocessIntermediateStorageManager.h"

#include "IntermediateStorage.h"
#include "SharedIntermediateStorage.h"
#include "logging.h"

const char* InterprocessIntermediateStorageManager::s_sharedMemoryNamePrefix = "iist_";

const char* InterprocessIntermediateStorageManager::s_intermediateStoragesKeyName =
	"intermediate_storages";

InterprocessIntermediateStorageManager::InterprocessIntermediateStorageManager(
	const std::string& instanceUuid, Id processId, bool isOwner)
	: BaseInterprocessDataManager(
		  s_sharedMemoryNamePrefix + std::to_string(processId) + "_" + instanceUuid,
		  3 * 1048576 /* 3 MB */,
		  instanceUuid,
		  processId,
		  isOwner)
	, m_insertsWithoutGrowth(0)
{
}

void InterprocessIntermediateStorageManager::pushIntermediateStorage(
	const std::shared_ptr<IntermediateStorage>& intermediateStorage)
{
	const size_t requiredInsertsToShrink = 10;

	const size_t overestimationMultiplier = 2;
	const size_t requiredSize = (intermediateStorage->getByteSize(sizeof(SharedMemory::String)) +
								 sizeof(SharedIntermediateStorage)) *
			overestimationMultiplier +
		1048576 /* 1 MB */;

	SharedMemory::ScopedAccess access(&m_sharedMemory);

	const size_t freeMemory = access.getFreeMemorySize();
	if (freeMemory < requiredSize)
	{
		const size_t requiredGrowth = requiredSize - freeMemory;

		LOG_INFO_STREAM(
			<< "grow memory - est: " << requiredSize << " size: " << access.getMemorySize()
			<< " free: " << access.getFreeMemorySize() << " alloc: " << requiredGrowth);

		access.growMemory(requiredGrowth);

		LOG_INFO("growing memory succeeded");

		m_insertsWithoutGrowth = 0;
	}
	else
	{
		m_insertsWithoutGrowth++;
	}

	SharedMemory::Queue<SharedIntermediateStorage>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(
			s_intermediateStoragesKeyName);
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
	storage.setStorageErrors(intermediateStorage->getErrors());

	storage.setNextId(intermediateStorage->getNextId());

	if (m_insertsWithoutGrowth >= requiredInsertsToShrink)
	{
		m_insertsWithoutGrowth = 0;

		LOG_INFO("shrinking shared memory");
		access.shrinkToFitMemory();
		LOG_INFO_STREAM(
			<< "shrunk memory - size: " << access.getMemorySize()
			<< " free: " << access.getFreeMemorySize());
	}

	LOG_INFO(access.logString());
}

std::shared_ptr<IntermediateStorage> InterprocessIntermediateStorageManager::popIntermediateStorage()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIntermediateStorage>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(
			s_intermediateStoragesKeyName);
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
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIntermediateStorage>>(
			s_intermediateStoragesKeyName);
	if (!queue)
	{
		return 0;
	}

	return queue->size();
}
