#include "data/indexer/IndexerCommandCxxManual.h"


std::string IndexerCommandCxxManual::getIndexerKindString()
{
	return "CxxManual";
}

IndexerCommandCxxManual::IndexerCommandCxxManual(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& excludedPaths,
	const std::string& languageStandard,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths,
	const std::vector<std::string>& compilerFlags
)
	: IndexerCommandCxx(sourceFilePath, indexedPaths, excludedPaths, systemHeaderSearchPaths, frameworkSearchPaths, compilerFlags)
	, m_languageStandard(languageStandard)
{
}

IndexerCommandCxxManual::~IndexerCommandCxxManual()
{
}

std::string IndexerCommandCxxManual::getKindString() const
{
	return getIndexerKindString();
}

std::string IndexerCommandCxxManual::getLanguageStandard() const
{
	return m_languageStandard;
}
