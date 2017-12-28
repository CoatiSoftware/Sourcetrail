#include "data/indexer/IndexerFactoryModuleCxxEmpty.h"

#include "data/indexer/IndexerCxx.h"
#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/parser/cxx/CxxParser.h"

std::shared_ptr<IndexerBase> IndexerFactoryModuleCxxEmpty::createIndexer()
{
	return std::make_shared<IndexerCxx<IndexerCommandCxxEmpty, CxxParser>>();
}
