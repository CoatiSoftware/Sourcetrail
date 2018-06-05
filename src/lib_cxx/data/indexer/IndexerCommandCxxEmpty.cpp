#include "data/indexer/IndexerCommandCxxEmpty.h"

#include <QJsonObject>

IndexerCommandType IndexerCommandCxxEmpty::getStaticIndexerCommandType()
{
	return INDEXER_COMMAND_CXX_EMPTY;
}

IndexerCommandCxxEmpty::IndexerCommandCxxEmpty(
	const FilePath& sourceFilePath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePathFilter>& excludeFilters,
	const std::set<FilePathFilter>& includeFilters,
	const FilePath& workingDirectory,
	const std::vector<FilePath>& systemHeaderSearchPaths,
	const std::vector<FilePath>& frameworkSearchPaths,
	const std::vector<std::wstring>& compilerFlags,
	const std::string& languageStandard
	)
	: IndexerCommandCxx(sourceFilePath, indexedPaths, excludeFilters, includeFilters, workingDirectory, systemHeaderSearchPaths, frameworkSearchPaths, compilerFlags)
	, m_languageStandard(languageStandard)
{
}

IndexerCommandType IndexerCommandCxxEmpty::getIndexerCommandType() const
{
	return getStaticIndexerCommandType();
}

size_t IndexerCommandCxxEmpty::getByteSize(size_t stringSize) const
{
	return IndexerCommandCxx::getByteSize(stringSize) + m_languageStandard.size();
}

std::string IndexerCommandCxxEmpty::getLanguageStandard() const
{
	return m_languageStandard;
}

QJsonObject IndexerCommandCxxEmpty::doSerialize() const
{
	QJsonObject jsonObject = IndexerCommandCxx::doSerialize();

	{
		jsonObject["language_standard"] = QString::fromStdString(m_languageStandard);
	}

	return jsonObject;
}
