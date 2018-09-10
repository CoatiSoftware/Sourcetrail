#include "LanguagePackageJava.h"

#include "data/indexer/IndexerJava.h"

std::vector<std::shared_ptr<IndexerBase>> LanguagePackageJava::instantiateSupportedIndexers() const
{
	return {
		std::make_shared<IndexerJava>()
	};
}
