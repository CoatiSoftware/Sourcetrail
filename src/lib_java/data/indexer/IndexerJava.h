#ifndef INDEXER_JAVA_H
#define INDEXER_JAVA_H

#include <memory>

#include "IndexerCommandJava.h"
#include "Indexer.h"

class IndexerJava: public Indexer<IndexerCommandJava>
{
public:
	virtual ~IndexerJava();
	std::shared_ptr<IntermediateStorage> doIndex(std::shared_ptr<IndexerCommandJava> indexerCommand) override;
};

#endif // INDEXER_JAVA_H
