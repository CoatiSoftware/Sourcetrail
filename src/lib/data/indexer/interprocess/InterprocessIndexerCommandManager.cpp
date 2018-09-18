#include "InterprocessIndexerCommandManager.h"

#include "IndexerCommand.h"
#include "logging.h"

const char* InterprocessIndexerCommandManager::s_sharedMemoryNamePrefix = "icmd_";

const char* InterprocessIndexerCommandManager::s_indexerCommandsKeyName = "indexer_commands";

InterprocessIndexerCommandManager::InterprocessIndexerCommandManager(const std::string& instanceUuid, Id processId, bool isOwner)
	: BaseInterprocessDataManager(s_sharedMemoryNamePrefix + instanceUuid, 1048576 /* 1 MB */, instanceUuid, processId, isOwner)
{
}

InterprocessIndexerCommandManager::~InterprocessIndexerCommandManager()
{
}

void InterprocessIndexerCommandManager::pushIndexerCommands(
	const std::vector<std::shared_ptr<IndexerCommand>>& indexerCommands)
{
	size_t size = 0;
	{
		const size_t overestimationMultiplier = 2;
		for (auto& command : indexerCommands)
		{
			size += command->getByteSize(sizeof(SharedMemory::String)) + sizeof(SharedIndexerCommand);
		}
		size *= overestimationMultiplier;
	}

	SharedMemory::ScopedAccess access(&m_sharedMemory);
	while (access.getFreeMemorySize() < size)
	{
		size_t currentSize = access.getMemorySize();
		LOG_INFO_STREAM(
			<< "grow memory - est: " << size << " size: " << currentSize
			<< " free: " << access.getFreeMemorySize() << " alloc: " << (currentSize));

		access.growMemory(currentSize);

		LOG_INFO("growing memory succeeded");
	}

	SharedMemory::Queue<SharedIndexerCommand>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIndexerCommand>>(s_indexerCommandsKeyName);
	if (!queue)
	{
		return;
	}

	for (auto& command : indexerCommands)
	{
		queue->push_back(SharedIndexerCommand(access.getAllocator()));
		SharedIndexerCommand& sharedCommand = queue->back();
		sharedCommand.fromLocal(command.get());
	}

	LOG_INFO(access.logString());
}

std::shared_ptr<IndexerCommand> InterprocessIndexerCommandManager::popIndexerCommand()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIndexerCommand>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIndexerCommand>>(s_indexerCommandsKeyName);
	if (!queue || !queue->size())
	{
		return nullptr;
	}

	std::shared_ptr<IndexerCommand> command = SharedIndexerCommand::fromShared(queue->front());

	queue->pop_front();

	return command;
}

void InterprocessIndexerCommandManager::clearIndexerCommands()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIndexerCommand>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIndexerCommand>>(s_indexerCommandsKeyName);
	if (!queue)
	{
		return;
	}

	queue->clear();
}

size_t InterprocessIndexerCommandManager::indexerCommandCount()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedIndexerCommand>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIndexerCommand>>(s_indexerCommandsKeyName);
	if (!queue)
	{
		return 0;
	}

	return queue->size();
}
