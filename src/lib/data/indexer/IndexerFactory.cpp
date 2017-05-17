#include "data/indexer/IndexerFactory.h"

#include "data/indexer/IndexerComposite.h"
#include "IndexerFactoryModule.h"

std::shared_ptr<IndexerFactory> IndexerFactory::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<IndexerFactory>(new IndexerFactory());
	}
	return s_instance;
}

void IndexerFactory::destroyInstance()
{
	s_instance.reset();
}


void IndexerFactory::addModule(std::shared_ptr<IndexerFactoryModule> module)
{
	m_modules.push_back(module);
}

std::shared_ptr<IndexerComposite> IndexerFactory::createCompositeIndexerForAllRegisteredModules()
{
	std::shared_ptr<IndexerComposite> composite = std::make_shared<IndexerComposite>();
	for (auto it: m_modules)
	{
		composite->addIndexer(it->createIndexer());
	}
	return composite;
}

std::shared_ptr<IndexerFactory> IndexerFactory::s_instance;

IndexerFactory::IndexerFactory()
{
}
