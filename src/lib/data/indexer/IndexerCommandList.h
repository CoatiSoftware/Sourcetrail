#ifndef INDEXER_COMMAND_LIST_H
#define INDEXER_COMMAND_LIST_H

#include <memory>
#include <mutex>
#include <deque>

#include "data/indexer/IndexerCommand.h"

class IndexerCommandList
{
public:
	void addCommand(std::shared_ptr<IndexerCommand> command);

	int size() const;

	void shuffle();

	std::shared_ptr<IndexerCommand> consumeCommand();

private:
	std::deque<std::shared_ptr<IndexerCommand>> m_commands;
	std::mutex m_commandsMutex;
};

#endif // INDEXER_COMMAND_LIST_H
