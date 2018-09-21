#ifndef INDEXER_JAVA_H
#define INDEXER_JAVA_H

#include <memory>

#include "Indexer.h"
#include "IndexerCommandJava.h"

struct IndexerStateInfo;

class IndexerJava: public Indexer<IndexerCommandJava>
{
public:
	IndexerJava();
	virtual ~IndexerJava();
	void interrupt() override;

private:
	std::shared_ptr<IntermediateStorage> doIndex(std::shared_ptr<IndexerCommandJava> indexerCommand) override;

	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
};

#endif // INDEXER_JAVA_H
