#ifndef INDEXER_JAVA_H
#define INDEXER_JAVA_H

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandJava.h"

class IndexerJava: public Indexer<IndexerCommandJava>
{
public:
	IndexerJava();
	virtual ~IndexerJava();

private:
	virtual std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommandJava> indexerCommand, std::shared_ptr<FileRegister> fileRegister);
};

#endif // INDEXER_JAVA_H
