#include "SharedIndexerCommand.h"

#include "IndexerCommandCxx.h"
#include "IndexerCommandJava.h"

#include "logging.h"
#include "utilityString.h"

void SharedIndexerCommand::fromLocal(IndexerCommand* indexerCommand)
{
	setSourceFilePath(indexerCommand->getSourceFilePath());

#if BUILD_CXX_LANGUAGE_PACKAGE
	if (dynamic_cast<IndexerCommandCxx*>(indexerCommand) != nullptr)
	{
		IndexerCommandCxx* cmd = dynamic_cast<IndexerCommandCxx*>(indexerCommand);

		setType(CXX);
		setIndexedPaths(cmd->getIndexedPaths());
		setExcludeFilters(cmd->getExcludeFilters());
		setIncludeFilters(cmd->getIncludeFilters());
		setWorkingDirectory(cmd->getWorkingDirectory());
		setCompilerFlags(cmd->getCompilerFlags());
		return;
	}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	if (dynamic_cast<IndexerCommandJava*>(indexerCommand) != nullptr)
	{
		IndexerCommandJava* cmd = dynamic_cast<IndexerCommandJava*>(indexerCommand);

		setType(JAVA);
		setLanguageStandard(cmd->getLanguageStandard());
		setClassPaths(cmd->getClassPath());
		return;
	}
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

	LOG_ERROR(
		L"Trying to push unhandled type of IndexerCommand for file: " +
		indexerCommand->getSourceFilePath().wstr() + L". Type string is: " +
		utility::decodeFromUtf8(indexerCommandTypeToString(indexerCommand->getIndexerCommandType())) +
		L". It will be ignored.");
}

std::shared_ptr<IndexerCommand> SharedIndexerCommand::fromShared(const SharedIndexerCommand& indexerCommand)
{
	switch (indexerCommand.getType())
	{
#if BUILD_CXX_LANGUAGE_PACKAGE
	case CXX:
		return std::make_shared<IndexerCommandCxx>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getIndexedPaths(),
			indexerCommand.getExcludeFilters(),
			indexerCommand.getIncludeFilters(),
			indexerCommand.getWorkingDirectory(),
			indexerCommand.getCompilerFlags());
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	case JAVA:
		return std::make_shared<IndexerCommandJava>(
			indexerCommand.getSourceFilePath(),
			indexerCommand.getLanguageStandard(),
			indexerCommand.getClassPaths());
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	case PYTHON:
		LOG_ERROR(
			L"Cannot convert shared IndexerCommand for file: " +
			indexerCommand.getSourceFilePath().wstr() + L". The type is unknown.");
		break;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
	default:
		LOG_ERROR(
			L"Cannot convert shared IndexerCommand for file: " +
			indexerCommand.getSourceFilePath().wstr() + L". The type is unknown.");
	}

	return nullptr;
}


SharedIndexerCommand::SharedIndexerCommand(SharedMemory::Allocator* allocator)
	: m_type(Type::UNKNOWN)
	, m_sourceFilePath("", allocator)
#if BUILD_CXX_LANGUAGE_PACKAGE
	, m_indexedPaths(allocator)
	, m_excludeFilters(allocator)
	, m_includeFilters(allocator)
	, m_workingDirectory("", allocator)
	, m_compilerFlags(allocator)
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	, m_languageStandard("", allocator)
	, m_classPaths(allocator)
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
{
}

SharedIndexerCommand::~SharedIndexerCommand() {}

FilePath SharedIndexerCommand::getSourceFilePath() const
{
	return FilePath(utility::decodeFromUtf8(m_sourceFilePath.c_str()));
}

void SharedIndexerCommand::setSourceFilePath(const FilePath& filePath)
{
	m_sourceFilePath = utility::encodeToUtf8(filePath.wstr()).c_str();
}

#if BUILD_CXX_LANGUAGE_PACKAGE

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

	for (const FilePathFilter& excludeFilter: excludeFilters)
	{
		SharedMemory::String path(m_excludeFilters.get_allocator());
		path = utility::encodeToUtf8(excludeFilter.wstr()).c_str();
		m_excludeFilters.push_back(path);
	}
}

std::set<FilePathFilter> SharedIndexerCommand::getIncludeFilters() const
{
	std::set<FilePathFilter> result;

	for (unsigned int i = 0; i < m_includeFilters.size(); i++)
	{
		result.insert(FilePathFilter(utility::decodeFromUtf8(m_includeFilters[i].c_str())));
	}

	return result;
}

void SharedIndexerCommand::setIncludeFilters(const std::set<FilePathFilter>& includeFilters)
{
	m_includeFilters.clear();

	for (const FilePathFilter& includeFilter: includeFilters)
	{
		SharedMemory::String path(m_includeFilters.get_allocator());
		path = utility::encodeToUtf8(includeFilter.wstr()).c_str();
		m_includeFilters.push_back(path);
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

std::vector<std::wstring> SharedIndexerCommand::getCompilerFlags() const
{
	std::vector<std::wstring> result;
	result.reserve(m_compilerFlags.size());

	for (unsigned int i = 0; i < m_compilerFlags.size(); i++)
	{
		result.push_back(utility::decodeFromUtf8(m_compilerFlags[i].c_str()));
	}

	return result;
}

void SharedIndexerCommand::setCompilerFlags(const std::vector<std::wstring>& compilerFlags)
{
	m_compilerFlags.clear();
	m_compilerFlags.reserve(compilerFlags.size());

	for (const std::wstring& compilerFlag: compilerFlags)
	{
		SharedMemory::String path(m_compilerFlags.get_allocator());
		path = utility::encodeToUtf8(compilerFlag).c_str();
		m_compilerFlags.push_back(path);
	}
}

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE

std::wstring SharedIndexerCommand::getLanguageStandard() const
{
	return utility::decodeFromUtf8(m_languageStandard.c_str());
}

void SharedIndexerCommand::setLanguageStandard(const std::wstring& languageStandard)
{
	m_languageStandard = utility::encodeToUtf8(languageStandard).c_str();
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

	for (const FilePath& classPath: classPaths)
	{
		SharedMemory::String path(m_classPaths.get_allocator());
		path = utility::encodeToUtf8(classPath.wstr()).c_str();
		m_classPaths.push_back(path);
	}
}

#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

SharedIndexerCommand::Type SharedIndexerCommand::getType() const
{
	return m_type;
}

void SharedIndexerCommand::setType(const SharedIndexerCommand::Type type)
{
	m_type = type;
}
