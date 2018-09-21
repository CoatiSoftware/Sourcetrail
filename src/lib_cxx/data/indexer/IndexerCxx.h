#ifndef INDEXER_CXX_H
#define INDEXER_CXX_H

#include <memory>
#include <mutex>

#include "Indexer.h"
#include "IndexerCommandCxx.h"

struct IndexerStateInfo;

class IndexerCxx: public Indexer<IndexerCommandCxx>
{
public:
	IndexerCxx();
	void interrupt() override;

private:
	std::shared_ptr<IntermediateStorage> doIndex(std::shared_ptr<IndexerCommandCxx> indexerCommand) override;

	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
};

#endif // INDEXER_CXX_H
