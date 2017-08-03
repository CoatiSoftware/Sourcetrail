#ifndef SHARED_INDEXER_COMMAND_H
#define SHARED_INDEXER_COMMAND_H

#include <set>

#include "utility/file/FilePath.h"
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

	std::set<FilePath> getExcludedPaths() const;
	void setExcludedPaths(const std::set<FilePath>& excludedPaths);

	FilePath getWorkingDirectory() const;
	void setWorkingDirectory(const FilePath& workingDirectory);

	std::string getLanguageStandard() const;
	void setLanguageStandard(const std::string& languageStandard);

	std::vector<std::string> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::string>& compilerFlags);

	std::vector<FilePath> getSystemHeaderSearchPaths() const;
	void setSystemHeaderSearchPaths(const std::vector<FilePath>& filePaths);

	std::vector<FilePath> getFrameworkSearchhPaths() const;
	void setFrameworkSearchhPaths(const std::vector<FilePath>& searchPaths);

	bool shouldApplyAnonymousTypedefTransformation() const;
	void setShouldApplyAnonymousTypedefTransformation(bool shouldApplyAnonymousTypedefTransformation);

	std::vector<FilePath> getClassPaths() const;
	void setClassPaths(const std::vector<FilePath>& classPaths);

private:
	enum Type
	{
		UNKNOWN = 0,
		CXX_CDB,
		CXX_MANUAL,
		JAVA
	};

	Type getType() const;
	void setType(const Type type);

	Type m_type;

	// indexer command
	SharedMemory::String m_sourceFilePath;
	SharedMemory::Vector<SharedMemory::String> m_indexedPaths;
	SharedMemory::Vector<SharedMemory::String> m_excludedPaths;

	// cxx
	SharedMemory::String m_workingDirectory;
	SharedMemory::String m_languageStandard;
	SharedMemory::Vector<SharedMemory::String> m_compilerFlags;
	SharedMemory::Vector<SharedMemory::String> m_systemHeaderSearchPaths;
	SharedMemory::Vector<SharedMemory::String> m_frameworkSearchPaths;
	bool m_shouldApplyAnonymousTypedefTransformation;

	// java
	SharedMemory::Vector<SharedMemory::String> m_classPaths;
};

#endif // SHARED_INDEXER_COMMAND_H
