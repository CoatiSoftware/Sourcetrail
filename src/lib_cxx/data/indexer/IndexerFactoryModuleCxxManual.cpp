#include "data/indexer/IndexerFactoryModuleCxxManual.h"

#include "data/indexer/IndexerCxxManual.h"

IndexerFactoryModuleCxxManual::~IndexerFactoryModuleCxxManual()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxManual::createIndexer()
{
	return std::make_shared<IndexerCxxManual>();
}
