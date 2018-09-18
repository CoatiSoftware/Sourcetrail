#ifndef SHARED_INDEXER_COMMAND_H
#define SHARED_INDEXER_COMMAND_H

#include <set>

#include "utility/file/FilePath.h"
#include "utility/file/FilePathFilter.h"
#include "utility/interprocess/SharedMemory.h"

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

	std::set<FilePath> getIndexedPaths() const;
	void setIndexedPaths(const std::set<FilePath>& indexedPaths);

	std::set<FilePathFilter> getExcludeFilters() const;
	void setExcludeFilters(const std::set<FilePathFilter>& excludeFilters);

	std::set<FilePathFilter> getIncludeFilters() const;
	void setIncludeFilters(const std::set<FilePathFilter>& includeFilters);

	FilePath getWorkingDirectory() const;
	void setWorkingDirectory(const FilePath& workingDirectory);

	std::wstring getLanguageStandard() const;
	void setLanguageStandard(const std::wstring& languageStandard);

	std::vector<std::wstring> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::wstring>& compilerFlags);

	std::vector<FilePath> getClassPaths() const;
	void setClassPaths(const std::vector<FilePath>& classPaths);

private:
	enum Type
	{
		UNKNOWN = 0,
		CXX,
		JAVA
	};

	Type getType() const;
	void setType(const Type type);

	Type m_type;

	// indexer command
	SharedMemory::String m_sourceFilePath;
	SharedMemory::Vector<SharedMemory::String> m_indexedPaths;
	SharedMemory::Vector<SharedMemory::String> m_excludeFilters;
	SharedMemory::Vector<SharedMemory::String> m_includeFilters;

	// cxx
	SharedMemory::String m_workingDirectory;
	SharedMemory::String m_languageStandard;
	SharedMemory::Vector<SharedMemory::String> m_compilerFlags;

	// java
	SharedMemory::Vector<SharedMemory::String> m_classPaths;
};

#endif // SHARED_INDEXER_COMMAND_H
