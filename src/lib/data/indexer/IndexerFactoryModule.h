#ifndef INDEXER_FACTORY_MODULE_H
#define INDEXER_FACTORY_MODULE_H

#include <memory>

#include "settings/LanguageType.h"

class IndexerBase;

class IndexerFactoryModule
{
public:
	virtual ~IndexerFactoryModule();
	virtual std::shared_ptr<IndexerBase> createIndexer() = 0;
};

#endif // INDEXER_FACTORY_MODULE_H
