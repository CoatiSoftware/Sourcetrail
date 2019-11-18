#include "LanguagePackageJava.h"

#include "IndexerJava.h"

std::vector<std::shared_ptr<IndexerBase>> LanguagePackageJava::instantiateSupportedIndexers() const
{
	return {std::make_shared<IndexerJava>()};
}
