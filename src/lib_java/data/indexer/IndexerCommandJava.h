#ifndef INDEXER_COMMAND_JAVA_H
#define INDEXER_COMMAND_JAVA_H

#include <vector>

#include "data/indexer/IndexerCommand.h"

class FilePath;

class IndexerCommandJava
	: public IndexerCommand
{
public:
	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandJava(
		const FilePath& sourceFilePath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths,
		const std::vector<FilePath>& classPath);
	virtual ~IndexerCommandJava();

	virtual IndexerCommandType getIndexerCommandType() const override;
	virtual size_t getByteSize() const override;

	std::vector<FilePath> getClassPath() const;

private:
	std::vector<FilePath> m_classPath;
};

#endif // INDEXER_COMMAND_JAVA_H
