#ifndef INDEXER_COMMAND_CXX_CDB_H
#define INDEXER_COMMAND_CXX_CDB_H

#include <vector>

#include "data/indexer/IndexerCommand.h"
#include "utility/file/FilePath.h"

namespace clang
{
	namespace tooling
	{
		struct CompileCommand;
	}
}

class IndexerCommandCxxCdb: public IndexerCommand
{
public:
	static std::string getIndexerKindString();

	IndexerCommandCxxCdb(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const FilePath& workingDirectory,
		const std::vector<std::string>& compilerFlags,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths);
	virtual ~IndexerCommandCxxCdb();

	virtual std::string getKindString() const;

	FilePath getWorkingDirectory() const;
	std::vector<std::string> getCompilerFlags() const;
	std::vector<FilePath> getSystemHeaderSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPaths() const;

private:
	FilePath m_workingDirectory;
	std::vector<std::string> m_compilerFlags;
	std::vector<FilePath> m_systemHeaderSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
};

#endif // INDEXER_COMMAND_CXX_CDB_H
