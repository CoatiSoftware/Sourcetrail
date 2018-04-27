#include "LanguagePackageCxx.h"

#include "data/indexer/IndexerCxx.h"
#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/parser/cxx/CxxParser.h"

std::vector<std::shared_ptr<IndexerBase>> LanguagePackageCxx::instantiateSupportedIndexers() const
{
	return { 
		std::make_shared<IndexerCxx<IndexerCommandCxxEmpty, CxxParser>>(),
		std::make_shared<IndexerCxx<IndexerCommandCxxCdb, CxxParser>>()
	};
}
