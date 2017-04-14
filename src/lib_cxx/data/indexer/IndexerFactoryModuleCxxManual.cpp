#include "data/indexer/IndexerFactoryModuleCxxManual.h"

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/parser/cxx/CxxParser.h"

IndexerFactoryModuleCxxManual::~IndexerFactoryModuleCxxManual()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxManual::createIndexer()
{
	return std::make_shared<Indexer<IndexerCommandCxxManual, CxxParser>>();
}
