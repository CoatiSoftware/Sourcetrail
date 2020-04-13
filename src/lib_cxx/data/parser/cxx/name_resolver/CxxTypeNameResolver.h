#ifndef CXX_TYPE_NAME_RESOLVER_H
#define CXX_TYPE_NAME_RESOLVER_H

#include "CxxNameResolver.h"
#include "../name/CxxTypeName.h"

class CxxTypeNameResolver: public CxxNameResolver
{
public:
	CxxTypeNameResolver(CanonicalFilePathCache* canonicalFilePathCache);
	CxxTypeNameResolver(const CxxNameResolver* other);

	std::unique_ptr<CxxTypeName> getName(const clang::QualType& qualType);
	std::unique_ptr<CxxTypeName> getName(const clang::Type* type);
};

#endif	  // CXX_TYPE_NAME_RESOLVER_H
