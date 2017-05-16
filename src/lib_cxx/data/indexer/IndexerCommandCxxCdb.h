#ifndef INDEXER_COMMAND_CXX_CDB_H
#define INDEXER_COMMAND_CXX_CDB_H

#include "data/indexer/IndexerCommandCxx.h"
#include "utility/file/FilePath.h"

namespace clang
{
	namespace tooling
	{
		struct CompileCommand;
	}
}

class IndexerCommandCxxCdb
	: public IndexerCommandCxx
{
public:
	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);

	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandCxxCdb(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const FilePath& workingDirectory,
		const std::vector<std::string>& compilerFlags,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths);
	virtual ~IndexerCommandCxxCdb();

	virtual IndexerCommandType getIndexerCommandType() const override;
	virtual size_t getByteSize() const override;

	FilePath getWorkingDirectory() const;

private:
	FilePath m_workingDirectory;
};

#endif // INDEXER_COMMAND_CXX_CDB_H
