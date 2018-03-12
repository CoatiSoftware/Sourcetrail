#include "SharedIndexerCommand.h"

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/indexer/IndexerCommandJava.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

void SharedIndexerCommand::fromLocal(IndexerCommand* indexerCommand)
{
	setSourceFilePath(indexerCommand->getSourceFilePath());
	setIndexedPaths(indexerCommand->getIndexedPaths());
	setExcludeFilters(indexerCommand->getExcludeFilters());

	if (dynamic_cast<IndexerCommandCxxCdb*>(indexerCommand) != nullptr)
	{
		IndexerCommandCxxCdb* cmd = dynamic_cast<IndexerCommandCxxCdb*>(indexerCommand);

		setType(CXX_CDB);
		setWorkingDirectory(cmd->getWorkingDirectory());
		setCompilerFlags(cmd->getCompilerFlags());
		setSystemHeaderSearchPaths(cmd->getSystemHeaderSearchPaths());
		setFrameworkSearchhPaths(cmd->getFrameworkSearchPaths());
	}
	else if (dynamic_cast<IndexerCommandCxxEmpty*>(indexerCommand) != nullptr)
	{
		IndexerCommandCxxEmpty* cmd = dynamic_cast<IndexerCommandCxxEmpty*>(indexerCommand);

		setType(CXX_EMPTY);
		setWorkingDirectory(cmd->getWorkingDirectory());
		setLanguageStandard(cmd->getLanguageStandard());
		setCompilerFlags(cmd->getCompilerFlags());
		setSystemHeaderSearchPaths(cmd->getSystemHeaderSearchPaths());
		setFrameworkSearchhPaths(cmd->getFrameworkSearchPaths());
	}
	else if (dynamic_cast<IndexerCommandJava*>(indexerCommand) != nullptr)
	{
		IndexerCommandJava* cmd = dynamic_cast<IndexerCommandJava*>(indexerCommand);

		setType(JAVA);
		setLanguageStandard(cmd->getLanguageStandard());
		setClassPaths(cmd->getClassPath());
	}
	else
	{
		LOG_ERROR(L"Trying to push unhandled type of IndexerCommand for file: " +
			indexerCommand->getSourceFilePath().wstr() + L". Type string is: " + 
			utility::decodeFromUtf8(indexerCommandTypeToString(indexerCommand->getIndexerCommandType())) +
			L". It will be ignored.");
	}
}

std::shared_ptr<IndexerCommand> SharedIndexerCommand::fromShared(const SharedIndexerCommand& indexerCommand)
{
	if (indexerCommand.getType() == CXX_CDB)
	{
		std::shared_ptr<IndexerCommand> command = std::make_shared<IndexerCommandCxxCdb>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludeFilters(),
			indexerCommand.getWorkingDirectory(),
			indexerCommand.getCompilerFlags(),
			indexerCommand.getSystemHeaderSearchPaths(),
			indexerCommand.getFrameworkSearchhPaths()
		);
		return command;
	}
	else if (indexerCommand.getType() == CXX_EMPTY)
	{
		std::shared_ptr<IndexerCommand> command = std::make_shared<IndexerCommandCxxEmpty>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludeFilters(),
			indexerCommand.getWorkingDirectory(),
			indexerCommand.getLanguageStandard(),
			indexerCommand.getSystemHeaderSearchPaths(),
			indexerCommand.getFrameworkSearchhPaths(),
			indexerCommand.getCompilerFlags()
		);
		return command;
	}
	else if (indexerCommand.getType() == JAVA)
	{
		return std::make_shared<IndexerCommandJava>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludeFilters(),
			indexerCommand.getLanguageStandard(),
			indexerCommand.getClassPaths()
		);
	}
	else
	{
		LOG_ERROR(L"Cannot convert shared IndexerCommand for file: " +
			indexerCommand.getSourceFilePath().wstr() + L". The type is unknown.");
	}

	return nullptr;
}


SharedIndexerCommand::SharedIndexerCommand(SharedMemory::Allocator* allocator)
	: m_type(Type::UNKNOWN)
	, m_sourceFilePath("", allocator)
	, m_indexedPaths(allocator)
	, m_excludeFilters(allocator)
	, m_workingDirectory("", allocator)
	, m_languageStandard("", allocator)
	, m_compilerFlags(allocator)
	, m_systemHeaderSearchPaths(allocator)
	, m_frameworkSearchPaths(allocator)
	, m_classPaths(allocator)
{
}

SharedIndexerCommand::~SharedIndexerCommand()
{
}

FilePath SharedIndexerCommand::getSourceFilePath() const
{
	return FilePath(utility::decodeFromUtf8(m_sourceFilePath.c_str()));
}

void SharedIndexerCommand::setSourceFilePath(const FilePath& filePath)
{
	m_sourceFilePath = utility::encodeToUtf8(filePath.wstr()).c_str();
}

std::set<FilePath> SharedIndexerCommand::getIndexedPaths() const
{
	std::set<FilePath> result;

	for (unsigned int i = 0; i < m_indexedPaths.size(); i++)
	{
		result.insert(FilePath(utility::decodeFromUtf8(m_indexedPaths[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setIndexedPaths(const std::set<FilePath>& indexedPaths)
{
	m_indexedPaths.clear();

	for (const FilePath& indexedPath: indexedPaths)
	{
		SharedMemory::String path(m_indexedPaths.get_allocator());
		path = utility::encodeToUtf8(indexedPath.wstr()).c_str();
		m_indexedPaths.push_back(path);
	}
}

std::set<FilePathFilter> SharedIndexerCommand::getExcludeFilters() const
{
	std::set<FilePathFilter> result;

	for (unsigned int i = 0; i < m_excludeFilters.size(); i++)
	{
		result.insert(FilePathFilter(utility::decodeFromUtf8(m_excludeFilters[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setExcludeFilters(const std::set<FilePathFilter>& excludeFilters)
{
	m_excludeFilters.clear();

	for (const FilePathFilter& excludeFilter : excludeFilters)
	{
		SharedMemory::String path(m_excludeFilters.get_allocator());
		path = utility::encodeToUtf8(excludeFilter.wstr()).c_str();
		m_excludeFilters.push_back(path);
	}
}

FilePath SharedIndexerCommand::getWorkingDirectory() const
{
	return FilePath(utility::decodeFromUtf8(m_workingDirectory.c_str()));
}

void SharedIndexerCommand::setWorkingDirectory(const FilePath& workingDirectory)
{
	m_workingDirectory = utility::encodeToUtf8(workingDirectory.wstr()).c_str();
}

std::string SharedIndexerCommand::getLanguageStandard() const
{
	return m_languageStandard.c_str();
}

void SharedIndexerCommand::setLanguageStandard(const std::string& languageStandard)
{
	m_languageStandard = languageStandard.c_str();
}

std::vector<std::wstring> SharedIndexerCommand::getCompilerFlags() const
{
	std::vector<std::wstring> result;
	result.reserve(m_compilerFlags.size());

	for (unsigned int i = 0; i <  m_compilerFlags.size(); i++)
	{
		result.push_back(utility::decodeFromUtf8(m_compilerFlags[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setCompilerFlags(const std::vector<std::wstring>& compilerFlags)
{
	m_compilerFlags.clear();
	m_compilerFlags.reserve(compilerFlags.size());

	for (const std::wstring& compilerFlag : compilerFlags)
	{
		SharedMemory::String path(m_compilerFlags.get_allocator());
		path = utility::encodeToUtf8(compilerFlag).c_str();
		m_compilerFlags.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getSystemHeaderSearchPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_systemHeaderSearchPaths.size());

	for (unsigned int i = 0; i < m_systemHeaderSearchPaths.size(); i++)
	{
		result.push_back(FilePath(utility::decodeFromUtf8(m_systemHeaderSearchPaths[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setSystemHeaderSearchPaths(const std::vector<FilePath>& filePaths)
{
	m_systemHeaderSearchPaths.clear();
	m_systemHeaderSearchPaths.reserve(filePaths.size());

	for (const FilePath& filePath : filePaths)
	{
		SharedMemory::String path(m_systemHeaderSearchPaths.get_allocator());
		path = utility::encodeToUtf8(filePath.wstr()).c_str();
		m_systemHeaderSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getFrameworkSearchhPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_frameworkSearchPaths.size());

	for (unsigned int i = 0; i < m_frameworkSearchPaths.size(); i++)
	{
		result.push_back(FilePath(utility::decodeFromUtf8(m_frameworkSearchPaths[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setFrameworkSearchhPaths(const std::vector<FilePath>& searchPaths)
{
	m_frameworkSearchPaths.clear();
	m_frameworkSearchPaths.reserve(searchPaths.size());

	for (const FilePath& searchPath : searchPaths)
	{
		SharedMemory::String path(m_frameworkSearchPaths.get_allocator());
		path = utility::encodeToUtf8(searchPath.wstr()).c_str();
		m_frameworkSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getClassPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_classPaths.size());

	for (unsigned int i = 0; i < m_classPaths.size(); i++)
	{
		result.push_back(FilePath(utility::decodeFromUtf8(m_classPaths[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setClassPaths(const std::vector<FilePath>& classPaths)
{
	m_classPaths.clear();
	m_classPaths.reserve(classPaths.size());

	for (const FilePath& classPath : classPaths)
	{
		SharedMemory::String path(m_classPaths.get_allocator());
		path = utility::encodeToUtf8(classPath.wstr()).c_str();
		m_classPaths.push_back(path);
	}
}

SharedIndexerCommand::Type SharedIndexerCommand::getType() const
{
	return m_type;
}

void SharedIndexerCommand::setType(const SharedIndexerCommand::Type type)
{
	m_type = type;
}
