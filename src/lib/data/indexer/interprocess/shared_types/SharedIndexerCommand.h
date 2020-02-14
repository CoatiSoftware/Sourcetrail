#ifndef SHARED_INDEXER_COMMAND_H
#define SHARED_INDEXER_COMMAND_H

#include <set>

#include "language_packages.h"

#include "FilePath.h"
#include "FilePathFilter.h"
#include "SharedMemory.h"

class IndexerCommand;

class SharedIndexerCommand
{
public:
	void fromLocal(IndexerCommand* indexerCommand);
	static std::shared_ptr<IndexerCommand> fromShared(const SharedIndexerCommand& indexerCommand);

	SharedIndexerCommand(SharedMemory::Allocator* allocator);
	~SharedIndexerCommand();

	FilePath getSourceFilePath() const;
	void setSourceFilePath(const FilePath& filePath);

#if BUILD_CXX_LANGUAGE_PACKAGE

	std::set<FilePath> getIndexedPaths() const;
	void setIndexedPaths(const std::set<FilePath>& indexedPaths);

	std::set<FilePathFilter> getExcludeFilters() const;
	void setExcludeFilters(const std::set<FilePathFilter>& excludeFilters);

	std::set<FilePathFilter> getIncludeFilters() const;
	void setIncludeFilters(const std::set<FilePathFilter>& includeFilters);

	FilePath getWorkingDirectory() const;
	void setWorkingDirectory(const FilePath& workingDirectory);

	std::vector<std::wstring> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::wstring>& compilerFlags);

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE

	std::wstring getLanguageStandard() const;
	void setLanguageStandard(const std::wstring& languageStandard);

	std::vector<FilePath> getClassPaths() const;
	void setClassPaths(const std::vector<FilePath>& classPaths);

#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

private:
	enum Type
	{
		UNKNOWN = 0,
#if BUILD_CXX_LANGUAGE_PACKAGE
		CXX,
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
		JAVA,
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
		PYTHON,
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
	};

	Type getType() const;
	void setType(const Type type);

	Type m_type;

	// indexer command
	SharedMemory::String m_sourceFilePath;

#if BUILD_CXX_LANGUAGE_PACKAGE
	SharedMemory::Vector<SharedMemory::String> m_indexedPaths;
	SharedMemory::Vector<SharedMemory::String> m_excludeFilters;
	SharedMemory::Vector<SharedMemory::String> m_includeFilters;
	SharedMemory::String m_workingDirectory;
	SharedMemory::Vector<SharedMemory::String> m_compilerFlags;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	SharedMemory::String m_languageStandard;
	SharedMemory::Vector<SharedMemory::String> m_classPaths;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
};

#endif	  // SHARED_INDEXER_COMMAND_H
