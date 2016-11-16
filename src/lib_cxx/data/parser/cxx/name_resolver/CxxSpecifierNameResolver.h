#ifndef CXX_SPECIFIER_NAME_RESOLVER_H
#define CXX_SPECIFIER_NAME_RESOLVER_H

#include <memory>

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CxxSpecifierNameResolver: public CxxNameResolver
{
public:
	CxxSpecifierNameResolver();
	CxxSpecifierNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	virtual ~CxxSpecifierNameResolver();

	std::shared_ptr<CxxName> getName(const clang::NestedNameSpecifier* nestedNameSpecifier);
};

#endif // CXX_SPECIFIER_NAME_RESOLVER_H
