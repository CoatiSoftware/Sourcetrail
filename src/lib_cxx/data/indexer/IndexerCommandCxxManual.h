#ifndef INDEXER_COMMAND_CXX_MANUAL_H
#define INDEXER_COMMAND_CXX_MANUAL_H

#include <vector>
#include <string>

#include "data/indexer/IndexerCommand.h"
#include "utility/file/FilePath.h"

class IndexerCommandCxxManual: public IndexerCommand
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

	std::string getLanguageStandard() const;
	std::vector<FilePath> getSystemHeaderSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<std::string> getCompilerFlags() const;

private:
	std::string m_languageStandard;
	std::vector<FilePath> m_systemHeaderSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	std::vector<std::string> m_compilerFlags;
};

#endif // INDEXER_COMMAND_CXX_MANUAL_H
