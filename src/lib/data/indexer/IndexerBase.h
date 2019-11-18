#ifndef INDEXER_BASE_H
#define INDEXER_BASE_H

#include <memory>
#include <string>

#include "IndexerCommandType.h"

class FileRegister;
class IndexerCommand;
class IntermediateStorage;

class IndexerBase
{
public:
	IndexerBase();
	virtual ~IndexerBase() = default;

	virtual IndexerCommandType getSupportedIndexerCommandType() const = 0;
	virtual std::shared_ptr<IntermediateStorage> index(
		std::shared_ptr<IndexerCommand> indexerCommand) = 0;
	virtual void interrupt() = 0;
};

#endif	  // INDEXER_BASE_H
