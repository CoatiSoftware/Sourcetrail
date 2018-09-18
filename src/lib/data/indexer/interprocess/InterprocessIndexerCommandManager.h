#ifndef INTERPROCESS_INDEXER_COMMAND_MANAGER_H
#define INTERPROCESS_INDEXER_COMMAND_MANAGER_H

#include "BaseInterprocessDataManager.h"
#include "SharedIndexerCommand.h"

class IndexerCommand;

class InterprocessIndexerCommandManager
	: public BaseInterprocessDataManager
{
public:
	InterprocessIndexerCommandManager(const std::string& instanceUuid, Id processId, bool isOwner);
	virtual ~InterprocessIndexerCommandManager();

	void pushIndexerCommands(const std::vector<std::shared_ptr<IndexerCommand>>& indexerCommands);
	std::shared_ptr<IndexerCommand> popIndexerCommand();

	void clearIndexerCommands();
	size_t indexerCommandCount();

private:
	static const char* s_sharedMemoryNamePrefix;
	static const char* s_indexerCommandsKeyName;
};

#endif // INTERPROCESS_INDEXER_COMMAND_MANAGER_H
