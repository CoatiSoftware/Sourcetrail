#ifndef INDEXER_FACTORY_MODULE_CXX_EMPTY_H
#define INDEXER_FACTORY_MODULE_CXX_EMPTY_H

#include "data/indexer/IndexerFactoryModule.h"

class IndexerFactoryModuleCxxEmpty: public IndexerFactoryModule
{
public:
	virtual std::shared_ptr<IndexerBase> createIndexer();
};

#endif // INDEXER_FACTORY_MODULE_CXX_EMPTY_H
