#ifndef INDEXER_COMMAND_CXX_H
#define INDEXER_COMMAND_CXX_H

#include <vector>
#include <string>

#include "data/indexer/IndexerCommand.h"

class FilePath;

class IndexerCommandCxx
	: public IndexerCommand
{
public:
	IndexerCommandCxx(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const FilePath& workingDirectory,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths,
		const std::vector<std::string>& compilerFlags);

	virtual size_t getByteSize(size_t stringSize) const override;

	std::vector<FilePath> getSystemHeaderSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<std::string> getCompilerFlags() const;
	FilePath getWorkingDirectory() const;


private:
	FilePath m_workingDirectory;
	std::vector<FilePath> m_systemHeaderSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	std::vector<std::string> m_compilerFlags;
};

#endif // INDEXER_COMMAND_CXXL_H
