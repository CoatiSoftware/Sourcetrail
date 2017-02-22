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
	: IndexerCommand(sourceFilePath, indexedPaths, excludedPaths)
	, m_languageStandard(languageStandard)
	, m_systemHeaderSearchPaths(systemHeaderSearchPaths)
	, m_frameworkSearchPaths(frameworkSearchPaths)
	, m_compilerFlags(compilerFlags)
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

std::vector<FilePath> IndexerCommandCxxManual::getSystemHeaderSearchPaths() const
{
	return m_systemHeaderSearchPaths;
}

std::vector<FilePath> IndexerCommandCxxManual::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<std::string> IndexerCommandCxxManual::getCompilerFlags() const
{
	return m_compilerFlags;
}
