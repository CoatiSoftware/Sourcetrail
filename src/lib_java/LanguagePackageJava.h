#ifndef LANGUAGE_PACKAGE_JAVA_H
#define LANGUAGE_PACKAGE_JAVA_H

#include "../lib/app/LanguagePackage.h"

class LanguagePackageJava: public LanguagePackage
{
public:
	virtual std::vector<std::shared_ptr<IndexerBase>> instantiateSupportedIndexers() const;
};

#endif	  // LANGUAGE_PACKAGE_JAVA_H
