#include "data/indexer/IndexerCommandList.h"

#include <algorithm>
#include <random>

void IndexerCommandList::addCommand(std::shared_ptr<IndexerCommand> command)
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	m_commands.push_back(command);
}

int IndexerCommandList::size() const
{
	return m_commands.size();
}

void IndexerCommandList::shuffle()
{
	srand(unsigned(time(NULL)));
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	std::random_shuffle(m_commands.begin(), m_commands.end());
}

std::shared_ptr<IndexerCommand> IndexerCommandList::consumeCommand()
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	std::shared_ptr<IndexerCommand> ret;
	if (!m_commands.empty())
	{
		ret = m_commands.front();
		m_commands.pop_front();
	}
	return ret;
}
