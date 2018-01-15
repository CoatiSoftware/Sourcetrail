#ifndef INDEXER_COMMAND_CXX_EMPTY_H
#define INDEXER_COMMAND_CXX_EMPTY_H

#include "data/indexer/IndexerCommandCxx.h"

class FilePath;

class IndexerCommandCxxEmpty
	: public IndexerCommandCxx
{
public:
	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandCxxEmpty(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const FilePath& workingDirectory,
		const std::string& languageStandard,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths,
		const std::vector<std::string>& compilerFlags);

	virtual IndexerCommandType getIndexerCommandType() const override;
	virtual size_t getByteSize(size_t stringSize) const override;

	std::string getLanguageStandard() const;

private:
	std::string m_languageStandard;
};

#endif // INDEXER_COMMAND_CXX_EMPTY_H
