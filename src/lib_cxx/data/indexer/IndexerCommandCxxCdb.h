#ifndef INDEXER_COMMAND_CXX_CDB_H
#define INDEXER_COMMAND_CXX_CDB_H

#include "data/indexer/IndexerCommandCxx.h"
#include "utility/file/FilePath.h"
#include "utility/file/FilePathFilter.h"

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
		const std::set<FilePathFilter>& excludeFilters,
		const FilePath& workingDirectory,
		const std::vector<std::wstring>& compilerFlags,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths);

	virtual IndexerCommandType getIndexerCommandType() const override;

	// this method only exists to avoid a linker problem on Linux
	const FilePath& getWorkingDirectory() const;
};

#endif // INDEXER_COMMAND_CXX_CDB_H
