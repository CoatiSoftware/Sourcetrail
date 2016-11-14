#ifndef CXX_SPECIFIER_NAME_RESOLVER_H
#define CXX_SPECIFIER_NAME_RESOLVER_H

#include "data/parser/cxx/name_resolver/CxxNameResolver.h"
#include "data/type/DataType.h"

class CxxSpecifierNameResolver: public CxxNameResolver
{
public:
	CxxSpecifierNameResolver();
	CxxSpecifierNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	virtual ~CxxSpecifierNameResolver();

	NameHierarchy getNameHierarchy(const clang::NestedNameSpecifier* nestedNameSpecifier);
};

#endif // CXX_SPECIFIER_NAME_RESOLVER_H
