#ifndef INDEXER_FACTORY_MODULE_CXX_MANUAL_H
#define INDEXER_FACTORY_MODULE_CXX_MANUAL_H

#include "data/indexer/IndexerFactoryModule.h"

class IndexerFactoryModuleCxxManual: public IndexerFactoryModule
{
public:
	virtual std::shared_ptr<IndexerBase> createIndexer();
};

#endif // INDEXER_FACTORY_MODULE_CXX_MANUAL_H
