#include "data/indexer/IndexerCommandCxxManual.h"

IndexerCommandType IndexerCommandCxxManual::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CXX_MANUAL;
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

IndexerCommandType IndexerCommandCxxManual::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandCxxManual::getByteSize() const
{
	return IndexerCommandCxx::getByteSize() + sizeof(std::string) + m_languageStandard.size();
}

std::string IndexerCommandCxxManual::getLanguageStandard() const
{
	return m_languageStandard;
}
