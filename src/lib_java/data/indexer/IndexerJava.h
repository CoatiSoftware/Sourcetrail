#ifndef INDEXER_JAVA_H
#define INDEXER_JAVA_H

#include <memory>

#include "data/indexer/IndexerCommandJava.h"
#include "data/indexer/Indexer.h"

class IndexerJava: public Indexer<IndexerCommandJava>
{
public:
	virtual ~IndexerJava();

	virtual std::shared_ptr<IntermediateStorage> doIndex(
		std::shared_ptr<IndexerCommandJava> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister);
};

#endif // INDEXER_JAVA_H
