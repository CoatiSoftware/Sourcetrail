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
		const std::set<FilePathFilter>& excludeFilters,
		const std::set<FilePathFilter>& includeFilters,
		const FilePath& workingDirectory,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths,
		const std::vector<std::wstring>& compilerFlags,
		const std::string& languageStandard);

	virtual IndexerCommandType getIndexerCommandType() const override;
	virtual size_t getByteSize(size_t stringSize) const override;

	std::string getLanguageStandard() const;

private:
	std::string m_languageStandard;
};

#endif // INDEXER_COMMAND_CXX_EMPTY_H
