#include "data/indexer/IndexerFactoryModuleCxxCdb.h"

#include "data/indexer/IndexerCxxCdb.h"

IndexerFactoryModuleCxxCdb::~IndexerFactoryModuleCxxCdb()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxCdb::createIndexer()
{
	return std::make_shared<IndexerCxxCdb>();
}
