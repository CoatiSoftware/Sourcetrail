#include "data/indexer/IndexerFactoryModuleCxxCdb.h"

#include "data/indexer/IndexerCxx.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/parser/cxx/CxxParser.h"

IndexerFactoryModuleCxxCdb::~IndexerFactoryModuleCxxCdb()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxCdb::createIndexer()
{
	return std::make_shared<IndexerCxx<IndexerCommandCxxCdb, CxxParser>>();
}
