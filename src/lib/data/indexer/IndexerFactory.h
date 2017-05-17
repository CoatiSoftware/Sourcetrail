#ifndef INDEXER_FACTORY_H
#define INDEXER_FACTORY_H

#include <memory>
#include <vector>

class IndexerBase;
class IndexerComposite;
class IndexerFactoryModule;

class IndexerFactory
{
public:
	static std::shared_ptr<IndexerFactory> getInstance();
	static void destroyInstance();

	void addModule(std::shared_ptr<IndexerFactoryModule> module);
	std::shared_ptr<IndexerComposite> createCompositeIndexerForAllRegisteredModules();

private:
	static std::shared_ptr<IndexerFactory> s_instance;
	IndexerFactory();

	std::vector<std::shared_ptr<IndexerFactoryModule>> m_modules;
};

#endif // INDEXER_FACTORY_H
