#ifndef INDEXER_COMMAND_H
#define INDEXER_COMMAND_H

#include <set>
#include <string>

#include "utility/file/FilePath.h"

class IndexerCommand
{
public:
	IndexerCommand(const FilePath& sourceFilePath, const std::set<FilePath>& indexedPaths, const std::set<FilePath>& excludedPaths);
	virtual ~IndexerCommand();

	virtual std::string getKindString() const = 0;

	FilePath getSourceFilePath() const;
	std::set<FilePath> getIndexedPaths() const;
	std::set<FilePath> getExcludedPath() const;

	bool cancelOnFatalErrors() const;
	void setCancelOnFatalErrors(bool cancelOnFatalErrors);

private:
	FilePath m_sourceFilePath;
	std::set<FilePath> m_indexedPaths;
	std::set<FilePath> m_excludedPaths;

	bool m_cancelOnFatalErrors;
};

#endif // INDEXER_COMMAND_H
