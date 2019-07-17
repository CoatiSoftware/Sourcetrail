#ifndef LANGUAGE_PACKAGE_H
#define LANGUAGE_PACKAGE_H

#include <memory>
#include <vector>

class IndexerBase;

class LanguagePackage
{
public:
	virtual ~LanguagePackage() = default;
	virtual std::vector<std::shared_ptr<IndexerBase>> instantiateSupportedIndexers() const = 0;
};

#endif // LANGUAGE_PACKAGE_H
