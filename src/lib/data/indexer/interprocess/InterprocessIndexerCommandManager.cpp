#include "InterprocessIndexerCommandManager.h"

#include "data/indexer/IndexerCommand.h"
#include "utility/logging/logging.h"

const char* InterprocessIndexerCommandManager::s_sharedMemoryNamePrefix = "icmd_";

const char* InterprocessIndexerCommandManager::s_indexerCommandsKeyName = "indexer_commands";

InterprocessIndexerCommandManager::InterprocessIndexerCommandManager(const std::string& instanceUuid, Id processId, bool isOwner)
	: BaseInterprocessDataManager(s_sharedMemoryNamePrefix + instanceUuid, 1048576 /* 1 MB */, instanceUuid, processId, isOwner)
{
}

InterprocessIndexerCommandManager::~InterprocessIndexerCommandManager()
{
}

void InterprocessIndexerCommandManager::setIndexerCommands(
	const std::vector<std::shared_ptr<IndexerCommand>>& indexerCommands)
{
	const unsigned int overestimationMultiplier = 3;

	size_t size = 1000;
	for (auto command : indexerCommands)
	{
		size += command->getByteSize() + sizeof(SharedIndexerCommand);
	}
	size *= overestimationMultiplier;


	SharedMemory::ScopedAccess access(&m_sharedMemory);

	size_t freeMemory = access.getFreeMemorySize();
	if (freeMemory <= size)
	{
		LOG_INFO_STREAM(
			<< "grow memory - est: " << size << " size: " << access.getMemorySize()
			<< " free: " << access.getFreeMemorySize() << " alloc: " << (size - freeMemory));

		access.growMemory(size - freeMemory);
	}

	SharedMemory::Queue<SharedIndexerCommand>* queue =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedIndexerCommand>>(s_indexerCommandsKeyName);
	if (!queue)
	{
		return;
	}

	for (auto command : indexerCommands)
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
