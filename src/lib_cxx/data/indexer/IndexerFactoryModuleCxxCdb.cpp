#include "data/indexer/IndexerFactoryModuleCxxCdb.h"

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/parser/cxx/CxxParser.h"

IndexerFactoryModuleCxxCdb::~IndexerFactoryModuleCxxCdb()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxCdb::createIndexer()
{
	return std::make_shared<Indexer<IndexerCommandCxxCdb, CxxParser>>();
}
