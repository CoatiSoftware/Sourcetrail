#ifndef INDEXER_CXX_H
#define INDEXER_CXX_H

#include "Indexer.h"
#include "IndexerCommandCxx.h"

class IndexerCxx: public Indexer<IndexerCommandCxx>
{
private:
	void doIndex(
		std::shared_ptr<IndexerCommandCxx> indexerCommand,
		std::shared_ptr<ParserClientImpl> parserClient,
		std::shared_ptr<IndexerStateInfo> m_indexerStateInfo) override;
};

#endif	  // INDEXER_CXX_H
