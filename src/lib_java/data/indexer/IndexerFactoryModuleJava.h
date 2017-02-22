#ifndef INDEXER_FACTORY_MODULE_JAVA_H
#define INDEXER_FACTORY_MODULE_JAVA_H

#include "data/indexer/IndexerFactoryModule.h"

class IndexerFactoryModuleJava: public IndexerFactoryModule
{
public:
	virtual ~IndexerFactoryModuleJava();
	virtual std::shared_ptr<IndexerBase> createIndexer();
};

#endif // INDEXER_FACTORY_MODULE_JAVA_H
