#ifndef INDEXER_FACTORY_MODULE_H
#define INDEXER_FACTORY_MODULE_H

#include <memory>

class IndexerBase;

class IndexerFactoryModule
{
public:
	virtual ~IndexerFactoryModule();
	virtual std::shared_ptr<IndexerBase> createIndexer() = 0;
};

#endif // INDEXER_FACTORY_MODULE_H
