#include "data/indexer/IndexerCommandList.h"

#include <algorithm>
#include <random>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"

void IndexerCommandList::addCommand(std::shared_ptr<IndexerCommand> command)
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);

	std::wstring commandHash = command->getSourceFilePath().wstr() + std::to_wstring(command->getByteSize(1));
	if (m_commandIndex.insert(commandHash).second == true) // Don't add duplicate indexer commands
	{
		m_commands.push_back(command);
	}
	else
	{
		LOG_WARNING(L"Duplicate indexer command was ignored: " + commandHash);
	}
}

size_t IndexerCommandList::size() const
{
	return m_commands.size();
}

void IndexerCommandList::shuffle()
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);

	typedef std::pair<unsigned long long int, std::shared_ptr<IndexerCommand>> PairType;

	std::vector<PairType> sourceFileSizesToCommands;
	for (std::shared_ptr<IndexerCommand> command: m_commands)
	{
		sourceFileSizesToCommands.push_back(std::make_pair(FileSystem::getFileByteSize(command->getSourceFilePath()), command));
	}
	std::sort(sourceFileSizesToCommands.begin(), sourceFileSizesToCommands.end(), [](const PairType& p, const PairType& q){ return p.first > q.first; });

	if (sourceFileSizesToCommands.size() > 2)
	{
		srand(unsigned(time(NULL)));
		std::random_shuffle(sourceFileSizesToCommands.begin(), sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() / 2);
		std::random_shuffle(sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() / 2, sourceFileSizesToCommands.end());
	}

	m_commands.clear();
	for (const PairType& pair: sourceFileSizesToCommands)
	{
		m_commands.push_back(pair.second);
	}
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

std::vector<std::shared_ptr<IndexerCommand>> IndexerCommandList::getAllCommands()
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	return std::vector<std::shared_ptr<IndexerCommand>>(m_commands.begin(), m_commands.end());
}
