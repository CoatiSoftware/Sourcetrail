#ifndef INDEXER_COMMAND_CXX_H
#define INDEXER_COMMAND_CXX_H

#include <vector>
#include <string>

#include "IndexerCommand.h"

class FilePath;

class IndexerCommandCxx
	: public IndexerCommand
{
public:
	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);
	static std::wstring getCompilerFlagLanguageStandard(const std::wstring &languageStandard);
	static std::vector<std::wstring> getCompilerFlagsForSystemHeaderSearchPaths(const std::vector<FilePath>& systemHeaderSearchPaths);
	static std::vector<std::wstring> getCompilerFlagsForFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandCxx(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePathFilter>& excludeFilters,
		const std::set<FilePathFilter>& includeFilters,
		const FilePath& workingDirectory,
		const std::vector<std::wstring>& compilerFlags);

	IndexerCommandType getIndexerCommandType() const override;
	size_t getByteSize(size_t stringSize) const override;

	const std::set<FilePath>& getIndexedPaths() const;
	const std::set<FilePathFilter>& getExcludeFilters() const;
	const std::set<FilePathFilter>& getIncludeFilters() const;
	const std::vector<std::wstring>& getCompilerFlags() const;
	const FilePath& getWorkingDirectory() const;

protected:
	QJsonObject doSerialize() const override;

private:
	std::set<FilePath> m_indexedPaths;
	std::set<FilePathFilter> m_excludeFilters;
	std::set<FilePathFilter> m_includeFilters;
	FilePath m_workingDirectory;
	std::vector<std::wstring> m_compilerFlags;
};

#endif // INDEXER_COMMAND_CXXL_H
