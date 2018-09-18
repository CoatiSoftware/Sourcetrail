#include "CombinedIndexerCommandProvider.h"

#include "utility.h"

void CombinedIndexerCommandProvider::addProvider(std::shared_ptr<IndexerCommandProvider> provider)
{
	if (provider)
	{
		m_providers.push_back(provider);
	}
	else
	{
		LOG_WARNING("Trying to add non-initialized indexer command provider.");
	}
}

std::vector<FilePath> CombinedIndexerCommandProvider::getAllSourceFilePaths() const
{
	size_t size = 0;
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		size += provider->size();
	}

	std::vector<FilePath> paths;
	paths.reserve(size);

	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		utility::append(paths, provider->getAllSourceFilePaths());
	}

	return paths;
}

std::shared_ptr<IndexerCommand> CombinedIndexerCommandProvider::consumeCommand()
{
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		std::shared_ptr<IndexerCommand> command = provider->consumeCommand();
		if (command)
		{
			return command;
		}
	}
	return std::shared_ptr<IndexerCommand>();
}

std::shared_ptr<IndexerCommand> CombinedIndexerCommandProvider::consumeCommandForSourceFilePath(const FilePath& filePath)
{
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		std::shared_ptr<IndexerCommand> command = provider->consumeCommandForSourceFilePath(filePath);
		if (command)
		{
			return command;
		}
	}
	return std::shared_ptr<IndexerCommand>();
}

std::vector<std::shared_ptr<IndexerCommand>> CombinedIndexerCommandProvider::consumeAllCommands()
{
	size_t size = 0;
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		size += provider->size();
	}

	std::vector<std::shared_ptr<IndexerCommand>> commands;
	commands.reserve(size);

	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		utility::append(commands, provider->consumeAllCommands());
	}
	return commands;
}

void CombinedIndexerCommandProvider::clear()
{
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		provider->clear();
	}
}

size_t CombinedIndexerCommandProvider::size() const
{
	size_t size = 0;
	for (const std::shared_ptr<IndexerCommandProvider>& provider : m_providers)
	{
		size += provider->size();
	}
	return size;
}
