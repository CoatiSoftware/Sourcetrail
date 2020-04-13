#include "LanguagePackageCxx.h"

#include "data/indexer/IndexerCxx.h"

std::vector<std::shared_ptr<IndexerBase>> LanguagePackageCxx::instantiateSupportedIndexers() const
{
	return {
		std::make_shared<IndexerCxx>(),
	};
}
