#ifndef INDEXER_JAVA_H
#define INDEXER_JAVA_H

#include "../../../lib/data/indexer/Indexer.h"
#include "IndexerCommandJava.h"

struct IndexerStateInfo;

class IndexerJava: public Indexer<IndexerCommandJava>
{
public:
	virtual ~IndexerJava();

private:
	void doIndex(
		std::shared_ptr<IndexerCommandJava> indexerCommand,
		std::shared_ptr<ParserClientImpl> parserClient,
		std::shared_ptr<IndexerStateInfo> m_indexerStateInfo) override;
};

#endif	  // INDEXER_JAVA_H
