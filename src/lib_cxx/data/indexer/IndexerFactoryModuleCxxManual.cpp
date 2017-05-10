#include "data/indexer/IndexerFactoryModuleCxxManual.h"

#include "data/indexer/IndexerCxx.h"
#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/parser/cxx/CxxParser.h"

IndexerFactoryModuleCxxManual::~IndexerFactoryModuleCxxManual()
{
}

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxManual::createIndexer()
{
	return std::make_shared<IndexerCxx<IndexerCommandCxxManual, CxxParser>>();
}
