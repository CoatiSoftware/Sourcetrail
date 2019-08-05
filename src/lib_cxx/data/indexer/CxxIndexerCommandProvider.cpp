#include "CxxIndexerCommandProvider.h"

#include "IndexerCommandCxx.h"
#include "logging.h"

CxxIndexerCommandProvider::CxxIndexerCommandProvider()
	: m_nextId(1)
{
}

void CxxIndexerCommandProvider::addCommand(const std::shared_ptr<IndexerCommandCxx>& command)
{
	std::shared_ptr<CommandRepresentation> representation = std::make_shared<CommandRepresentation>();

	{
		for (const FilePath& indexedPath : command->getIndexedPaths())
		{
			std::map<FilePath, Id>::const_iterator it = m_indexedPathsToIds.find(indexedPath);
			if (it != m_indexedPathsToIds.end())
			{
				representation->m_indexedPathIds.emplace(it->second);
			}
			else
			{
				const Id id = getId();
				m_indexedPathsToIds[indexedPath] = id;
				m_idsToIndexedPaths[id] = indexedPath;
				representation->m_indexedPathIds.emplace(id);
			}
		}
	}

	{
		for (const FilePathFilter& excludeFilter : command->getExcludeFilters())
		{
			std::map<std::wstring, Id>::const_iterator it = m_excludeFiltersToIds.find(excludeFilter.wstr());
			if (it != m_excludeFiltersToIds.end())
			{
				representation->m_excludeFilterIds.emplace(it->second);
			}
			else
			{
				const Id id = getId();
				m_excludeFiltersToIds[excludeFilter.wstr()] = id;
				m_idsToExcludeFilters[id] = excludeFilter.wstr();
				representation->m_excludeFilterIds.emplace(id);
			}
		}
	}

	{
		for (const FilePathFilter& includeFilter : command->getIncludeFilters())
		{
			std::map<std::wstring, Id>::const_iterator it = m_includeFiltersToIds.find(includeFilter.wstr());
			if (it != m_includeFiltersToIds.end())
			{
				representation->m_includeFilterIds.emplace(it->second);
			}
			else
			{
				const Id id = getId();
				m_includeFiltersToIds[includeFilter.wstr()] = id;
				m_idsToIncludeFilters[id] = includeFilter.wstr();
				representation->m_includeFilterIds.emplace(id);
			}
		}
	}

	{
		const FilePath workingDirectory = command->getWorkingDirectory();
		std::map<FilePath, Id>::const_iterator it = m_workingDirectoriesToIds.find(workingDirectory);
		if (it != m_workingDirectoriesToIds.end())
		{
			representation->m_workingDirectoryId = it->second;
		}
		else
		{
			const Id id = getId();
			m_workingDirectoriesToIds[workingDirectory] = id;
			m_idsToWorkingDirectories[id] = workingDirectory;
			representation->m_workingDirectoryId = id;
		}
	}

	{
		const std::vector<std::wstring>& compilerFlags = command->getCompilerFlags();
		representation->m_compilerFlagIds.reserve(compilerFlags.size());
		for (const std::wstring& compilerFlag : compilerFlags)
		{
			std::unordered_map<std::wstring, Id>::const_iterator it = m_compilerFlagsToIds.find(compilerFlag);
			if (it != m_compilerFlagsToIds.end())
			{
				representation->m_compilerFlagIds.emplace_back(it->second);
			}
			else
			{
				const Id id = getId();
				m_compilerFlagsToIds.emplace(compilerFlag, id);
				m_idsToCompilerFlags.emplace(id, compilerFlag);
				representation->m_compilerFlagIds.emplace_back(id);
			}
		}
	}

	m_commands.emplace(command->getSourceFilePath(), representation);
}

std::vector<FilePath> CxxIndexerCommandProvider::getAllSourceFilePaths() const
{
	std::vector<FilePath> paths;
	paths.reserve(m_commands.size());

	for (std::map<FilePath, std::shared_ptr<CommandRepresentation>>::const_iterator it = m_commands.begin(); it != m_commands.end(); it++)
	{
		paths.emplace_back(it->first);
	}

	return paths;
}

std::shared_ptr<IndexerCommand> CxxIndexerCommandProvider::consumeCommand()
{
	if (!m_commands.empty())
	{
		std::map<FilePath, std::shared_ptr<CommandRepresentation>>::const_iterator it = m_commands.begin();
		if (it->second)
		{
			std::shared_ptr<IndexerCommand> command = represetationToCommand(it->first, it->second);
			m_commands.erase(it);
			return command;
		}
	}
	return std::shared_ptr<IndexerCommand>();
}

std::shared_ptr<IndexerCommand> CxxIndexerCommandProvider::consumeCommandForSourceFilePath(const FilePath& filePath)
{
	std::map<FilePath, std::shared_ptr<CommandRepresentation>>::const_iterator it = m_commands.find(filePath);
	if (it != m_commands.end() && it->second)
	{
		std::shared_ptr<IndexerCommand> command = represetationToCommand(it->first, it->second);
		m_commands.erase(it);
		return command;
	}
	return std::shared_ptr<IndexerCommand>();
}

std::vector<std::shared_ptr<IndexerCommand>> CxxIndexerCommandProvider::consumeAllCommands()
{
	std::vector<std::shared_ptr<IndexerCommand>> commands;
	commands.reserve(m_commands.size());
	for (std::map<FilePath, std::shared_ptr<CommandRepresentation>>::const_iterator it = m_commands.begin(); it != m_commands.end(); it++)
	{
		commands.emplace_back(represetationToCommand(it->first, it->second));
	}
	m_commands.clear();
	return commands;
}

void CxxIndexerCommandProvider::clear()
{
	m_commands.clear();
}

size_t CxxIndexerCommandProvider::size() const
{
	return m_commands.size();
}

void CxxIndexerCommandProvider::logStats() const
{
	LOG_INFO("CxxIndexerCommandProvider stats:");
	LOG_INFO("\tindexed path count: " + std::to_string(m_idsToIndexedPaths.size()));
	LOG_INFO("\texclude filter count: " + std::to_string(m_idsToExcludeFilters.size()));
	LOG_INFO("\tinclude filter count: " + std::to_string(m_idsToIncludeFilters.size()));
	LOG_INFO("\tworking directory count: " + std::to_string(m_idsToWorkingDirectories.size()));
	LOG_INFO("\tcompiler flag count: " + std::to_string(m_idsToCompilerFlags.size()));
}

Id CxxIndexerCommandProvider::getId()
{
	return m_nextId++;
}

std::shared_ptr<IndexerCommandCxx> CxxIndexerCommandProvider::represetationToCommand(const FilePath& sourceFilePath, std::shared_ptr<CommandRepresentation> representation)
{
	std::set<FilePath> indexedPaths;
	for (const Id id : representation->m_indexedPathIds)
	{
		indexedPaths.insert(m_idsToIndexedPaths[id]);
	}

	std::set<FilePathFilter> excludeFilters;
	for (const Id id : representation->m_excludeFilterIds)
	{
		excludeFilters.insert(FilePathFilter(m_idsToExcludeFilters[id]));
	}

	std::set<FilePathFilter> includeFilters;
	for (const Id id : representation->m_includeFilterIds)
	{
		includeFilters.insert(FilePathFilter(m_idsToIncludeFilters[id]));
	}

	FilePath workingDirectory = m_idsToWorkingDirectories[representation->m_workingDirectoryId];

	std::vector<std::wstring> compilerFlags;
	for (const Id id : representation->m_compilerFlagIds)
	{
		compilerFlags.push_back(m_idsToCompilerFlags[id]);
	}

	return std::make_shared<IndexerCommandCxx>(
		sourceFilePath,
		indexedPaths,
		excludeFilters,
		includeFilters,
		workingDirectory,
		compilerFlags
	);
}
