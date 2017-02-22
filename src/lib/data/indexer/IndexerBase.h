#ifndef INDEXER_BASE_H
#define INDEXER_BASE_H

#include <memory>

#include "data/indexer/IndexerCommand.h"
#include "data/IntermediateStorage.h"

class FileRegister;

class IndexerBase
{
public:
	IndexerBase();
	virtual ~IndexerBase();

	virtual std::string getKindString() const = 0;

	virtual std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister) = 0;

	virtual void interrupt();

	bool interrupted() const;

private:
	bool m_interrupted;
};

#endif // INDEXER_BASE_H
