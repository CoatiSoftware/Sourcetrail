#include "data/indexer/IndexerFactoryModuleJava.h"

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaParser.h"

IndexerFactoryModuleJava::~IndexerFactoryModuleJava()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleJava::createIndexer()
{
	return std::make_shared<Indexer<IndexerCommandJava, JavaParser>>();
}
