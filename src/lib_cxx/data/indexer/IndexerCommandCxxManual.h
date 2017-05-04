#ifndef INDEXER_COMMAND_CXX_MANUAL_H
#define INDEXER_COMMAND_CXX_MANUAL_H

#include "data/indexer/IndexerCommandCxx.h"

class FilePath;

class IndexerCommandCxxManual
	: public IndexerCommandCxx
{
public:
	static std::string getIndexerKindString();

	IndexerCommandCxxManual(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const std::string& languageStandard,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths,
		const std::vector<std::string>& compilerFlags);

	virtual ~IndexerCommandCxxManual();

	virtual std::string getKindString() const;
	virtual size_t getByteSize() const;

	std::string getLanguageStandard() const;

private:
	std::string m_languageStandard;
};

#endif // INDEXER_COMMAND_CXX_MANUAL_H
