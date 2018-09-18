#ifndef CXX_SPECIFIER_NAME_RESOLVER_H
#define CXX_SPECIFIER_NAME_RESOLVER_H

#include <memory>

#include "CxxNameResolver.h"

class CxxName;

class CxxSpecifierNameResolver: public CxxNameResolver
{
public:
	CxxSpecifierNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
	CxxSpecifierNameResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);

	std::shared_ptr<CxxName> getName(const clang::NestedNameSpecifier* nestedNameSpecifier);
};

#endif // CXX_SPECIFIER_NAME_RESOLVER_H
