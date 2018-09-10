#ifndef INDEXER_CXX_H
#define INDEXER_CXX_H

#include <memory>

#include "data/indexer/IndexerCommandCxx.h"
#include "data/indexer/Indexer.h"

class IndexerCxx: public Indexer<IndexerCommandCxx>
{
public:
	virtual ~IndexerCxx() = default;
	virtual std::shared_ptr<IntermediateStorage> doIndex(std::shared_ptr<IndexerCommandCxx> indexerCommand);
};

#endif // INDEXER_CXX_H
