#ifndef INDEXER_COMMAND_CXX_H
#define INDEXER_COMMAND_CXX_H

#include <vector>
#include <string>

#include "data/indexer/IndexerCommand.h"
#include "utility/file/FilePath.h"

class IndexerCommandCxx
	: public IndexerCommand
{
public:
	IndexerCommandCxx(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths,
		const std::vector<std::string>& compilerFlags);

	virtual ~IndexerCommandCxx();

	std::vector<FilePath> getSystemHeaderSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<std::string> getCompilerFlags() const;

	bool preprocessorOnly() const override;
	void setPreprocessorOnly(bool preprocessorOnly) override;

private:
	std::vector<FilePath> m_systemHeaderSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	std::vector<std::string> m_compilerFlags;

	bool m_preprocessorOnly;
};

#endif // INDEXER_COMMAND_CXXL_H
