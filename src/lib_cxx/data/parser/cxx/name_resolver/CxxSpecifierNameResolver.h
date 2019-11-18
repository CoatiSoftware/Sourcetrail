#ifndef CXX_SPECIFIER_NAME_RESOLVER_H
#define CXX_SPECIFIER_NAME_RESOLVER_H

#include <memory>

#include "CxxNameResolver.h"

class CxxName;

class CxxSpecifierNameResolver: public CxxNameResolver
{
public:
	CxxSpecifierNameResolver(CanonicalFilePathCache* canonicalFilePathCache);
	CxxSpecifierNameResolver(const CxxNameResolver* other);

	std::unique_ptr<CxxName> getName(const clang::NestedNameSpecifier* nestedNameSpecifier);
};

#endif	  // CXX_SPECIFIER_NAME_RESOLVER_H
