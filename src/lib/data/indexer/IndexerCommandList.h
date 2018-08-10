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

	size_t size() const;

	void sort();

	std::shared_ptr<IndexerCommand> consumeCommand();

	std::vector<std::shared_ptr<IndexerCommand>> getAllCommands();

private:
	std::deque<std::shared_ptr<IndexerCommand>> m_commands;
	std::mutex m_commandsMutex;

	std::set<std::wstring> m_commandIndex;
};

#endif // INDEXER_COMMAND_LIST_H
