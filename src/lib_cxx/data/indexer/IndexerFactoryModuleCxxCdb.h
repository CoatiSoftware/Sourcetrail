#ifndef INDEXER_FACTORY_MODULE_CXX_CDB_H
#define INDEXER_FACTORY_MODULE_CXX_CDB_H

#include "data/indexer/IndexerFactoryModule.h"

class IndexerFactoryModuleCxxCdb: public IndexerFactoryModule
{
public:
	virtual ~IndexerFactoryModuleCxxCdb();
	virtual std::shared_ptr<IndexerBase> createIndexer();
};

#endif // INDEXER_FACTORY_MODULE_CXX_CDB_H
