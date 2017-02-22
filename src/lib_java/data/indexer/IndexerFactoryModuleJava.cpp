#include "data/indexer/IndexerFactoryModuleJava.h"

#include "data/indexer/IndexerJava.h"

IndexerFactoryModuleJava::~IndexerFactoryModuleJava()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleJava::createIndexer()
{
	return std::make_shared<IndexerJava>();
}
