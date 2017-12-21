#include "SharedIndexerCommand.h"

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/indexer/IndexerCommandJava.h"

#include "utility/logging/logging.h"

void SharedIndexerCommand::fromLocal(IndexerCommand* indexerCommand)
{
	setSourceFilePath(indexerCommand->getSourceFilePath());
	setIndexedPaths(indexerCommand->getIndexedPaths());
	setExcludedPaths(indexerCommand->getExcludedPath());

	if (dynamic_cast<IndexerCommandCxxCdb*>(indexerCommand) != NULL)
	{
		IndexerCommandCxxCdb* cmd = dynamic_cast<IndexerCommandCxxCdb*>(indexerCommand);

		setType(CXX_CDB);
		setWorkingDirectory(cmd->getWorkingDirectory());
		setCompilerFlags(cmd->getCompilerFlags());
		setSystemHeaderSearchPaths(cmd->getSystemHeaderSearchPaths());
		setFrameworkSearchhPaths(cmd->getFrameworkSearchPaths());
	}
	else if (dynamic_cast<IndexerCommandCxxManual*>(indexerCommand) != NULL)
	{
		IndexerCommandCxxManual* cmd = dynamic_cast<IndexerCommandCxxManual*>(indexerCommand);

		setType(CXX_MANUAL);
		setLanguageStandard(cmd->getLanguageStandard());
		setCompilerFlags(cmd->getCompilerFlags());
		setSystemHeaderSearchPaths(cmd->getSystemHeaderSearchPaths());
		setFrameworkSearchhPaths(cmd->getFrameworkSearchPaths());
	}
	else if (dynamic_cast<IndexerCommandJava*>(indexerCommand) != NULL)
	{
		IndexerCommandJava* cmd = dynamic_cast<IndexerCommandJava*>(indexerCommand);

		setType(JAVA);
		setLanguageStandard(cmd->getLanguageStandard());
		setClassPaths(cmd->getClassPath());
	}
	else
	{
		LOG_ERROR_STREAM(<< "Trying to push unhandled type of IndexerCommand for file: "
			<< indexerCommand->getSourceFilePath().str() << ". Type string is: " << indexerCommandTypeToString(indexerCommand->getIndexerCommandType())
			<< ". It will be ignored.");
	}
}

std::shared_ptr<IndexerCommand> SharedIndexerCommand::fromShared(const SharedIndexerCommand& indexerCommand)
{
	if (indexerCommand.getType() == CXX_CDB)
	{
		std::shared_ptr<IndexerCommand> command = std::make_shared<IndexerCommandCxxCdb>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludedPaths(),
			indexerCommand.getWorkingDirectory(),
			indexerCommand.getCompilerFlags(),
			indexerCommand.getSystemHeaderSearchPaths(),
			indexerCommand.getFrameworkSearchhPaths()
		);
		return command;
	}
	else if (indexerCommand.getType() == CXX_MANUAL)
	{
		std::shared_ptr<IndexerCommand> command = std::make_shared<IndexerCommandCxxManual>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludedPaths(),
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
			indexerCommand.getExcludedPaths(),
			indexerCommand.getLanguageStandard(),
			indexerCommand.getClassPaths()
		);
	}
	else
	{
		LOG_ERROR_STREAM(<< "Cannot convert shared IndexerCommand for file: "
			<< indexerCommand.getSourceFilePath().str() << ". The type is unknown.");
	}

	return nullptr;
}


SharedIndexerCommand::SharedIndexerCommand(SharedMemory::Allocator* allocator)
	: m_type(Type::UNKNOWN)
	, m_sourceFilePath("", allocator)
	, m_indexedPaths(allocator)
	, m_excludedPaths(allocator)
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
	return FilePath(m_sourceFilePath.c_str());
}

void SharedIndexerCommand::setSourceFilePath(const FilePath& filePath)
{
	m_sourceFilePath = filePath.str().c_str();
}

std::set<FilePath> SharedIndexerCommand::getIndexedPaths() const
{
	std::set<FilePath> result;

	for (unsigned int i = 0; i < m_indexedPaths.size(); i++)
	{
		result.insert(FilePath(m_indexedPaths[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setIndexedPaths(const std::set<FilePath>& indexedPaths)
{
	m_indexedPaths.clear();

	for (std::set<FilePath>::iterator it = indexedPaths.begin(); it != indexedPaths.end(); it++)
	{
		SharedMemory::String path(m_indexedPaths.get_allocator());
		path = (*it).str().c_str();
		m_indexedPaths.push_back(path);
	}
}

std::set<FilePath> SharedIndexerCommand::getExcludedPaths() const
{
	std::set<FilePath> result;

	for (unsigned int i = 0; i < m_excludedPaths.size(); i++)
	{
		result.insert(FilePath(m_excludedPaths[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setExcludedPaths(const std::set<FilePath>& excludedPaths)
{
	m_excludedPaths.clear();

	for (std::set<FilePath>::iterator it = excludedPaths.begin(); it != excludedPaths.end(); it++)
	{
		SharedMemory::String path(m_excludedPaths.get_allocator());
		path = (*it).str().c_str();
		m_excludedPaths.push_back(path);
	}
}

FilePath SharedIndexerCommand::getWorkingDirectory() const
{
	return FilePath(m_workingDirectory.c_str());
}

void SharedIndexerCommand::setWorkingDirectory(const FilePath& workingDirectory)
{
	m_workingDirectory = workingDirectory.str().c_str();
}

std::string SharedIndexerCommand::getLanguageStandard() const
{
	return m_languageStandard.c_str();
}

void SharedIndexerCommand::setLanguageStandard(const std::string& languageStandard)
{
	m_languageStandard = languageStandard.c_str();
}

std::vector<std::string> SharedIndexerCommand::getCompilerFlags() const
{
	std::vector<std::string> result;
	result.reserve(m_compilerFlags.size());

	for (unsigned int i = 0; i <  m_compilerFlags.size(); i++)
	{
		result.push_back(m_compilerFlags[i].c_str());
	}

	return result;
}

void SharedIndexerCommand::setCompilerFlags(const std::vector<std::string>& compilerFlags)
{
	m_compilerFlags.clear();
	m_compilerFlags.reserve(compilerFlags.size());

	for (unsigned int i = 0; i < compilerFlags.size(); i++)
	{
		SharedMemory::String path(m_compilerFlags.get_allocator());
		path = compilerFlags[i].c_str();
		m_compilerFlags.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getSystemHeaderSearchPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_systemHeaderSearchPaths.size());

	for (unsigned int i = 0; i < m_systemHeaderSearchPaths.size(); i++)
	{
		result.push_back(FilePath(m_systemHeaderSearchPaths[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setSystemHeaderSearchPaths(const std::vector<FilePath>& filePaths)
{
	m_systemHeaderSearchPaths.clear();
	m_systemHeaderSearchPaths.reserve(filePaths.size());

	for (unsigned int i = 0; i < filePaths.size(); i++)
	{
		SharedMemory::String path(m_systemHeaderSearchPaths.get_allocator());
		path = filePaths[i].str().c_str();
		m_systemHeaderSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getFrameworkSearchhPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_frameworkSearchPaths.size());

	for (unsigned int i = 0; i < m_frameworkSearchPaths.size(); i++)
	{
		result.push_back(FilePath(m_frameworkSearchPaths[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setFrameworkSearchhPaths(const std::vector<FilePath>& searchPaths)
{
	m_frameworkSearchPaths.clear();
	m_frameworkSearchPaths.reserve(searchPaths.size());

	for (unsigned int i = 0; i < searchPaths.size(); i++)
	{
		SharedMemory::String path(m_frameworkSearchPaths.get_allocator());
		path = searchPaths[i].str().c_str();
		m_frameworkSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedIndexerCommand::getClassPaths() const
{
	std::vector<FilePath> result;
	result.reserve(m_classPaths.size());

	for (unsigned int i = 0; i < m_classPaths.size(); i++)
	{
		result.push_back(FilePath(m_classPaths[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setClassPaths(const std::vector<FilePath>& classPaths)
{
	m_classPaths.clear();
	m_classPaths.reserve(classPaths.size());

	for (unsigned int i = 0; i < classPaths.size(); i++)
	{
		SharedMemory::String path(m_classPaths.get_allocator());
		path = classPaths[i].str().c_str();
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
