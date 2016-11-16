#ifndef CXX_TYPE_NAME_RESOLVER_H
#define CXX_TYPE_NAME_RESOLVER_H

#include "data/parser/cxx/name/CxxTypeName.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CxxTypeNameResolver: public CxxNameResolver
{
public:
	CxxTypeNameResolver();
	CxxTypeNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	virtual ~CxxTypeNameResolver();

	std::shared_ptr<CxxTypeName> getName(const clang::QualType& qualType);
	std::shared_ptr<CxxTypeName> getName(const clang::Type* type);
};

#endif // CXX_TYPE_NAME_RESOLVER_H
