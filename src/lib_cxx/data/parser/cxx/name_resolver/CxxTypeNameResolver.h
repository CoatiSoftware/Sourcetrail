#ifndef CXX_TYPE_NAME_RESOLVER_H
#define CXX_TYPE_NAME_RESOLVER_H

#include "CxxTypeName.h"
#include "CxxNameResolver.h"

class CxxTypeNameResolver: public CxxNameResolver
{
public:
	CxxTypeNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
	CxxTypeNameResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);

	std::shared_ptr<CxxTypeName> getName(const clang::QualType& qualType);
	std::shared_ptr<CxxTypeName> getName(const clang::Type* type);
};

#endif // CXX_TYPE_NAME_RESOLVER_H
