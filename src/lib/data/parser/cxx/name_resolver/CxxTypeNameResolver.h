#ifndef CXX_TYPE_NAME_RESOLVER_H
#define CXX_TYPE_NAME_RESOLVER_H

#include "data/parser/cxx/name_resolver/CxxNameResolver.h"
#include "data/type/DataType.h"

class CxxTypeNameResolver: public CxxNameResolver
{
public:
	CxxTypeNameResolver();
	CxxTypeNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	~CxxTypeNameResolver();

	std::shared_ptr<DataType> qualTypeToDataType(clang::QualType qualType);

private:
	std::shared_ptr<DataType> typeToDataType(const clang::Type* type);
};

#endif // CXX_TYPE_NAME_RESOLVER_H
