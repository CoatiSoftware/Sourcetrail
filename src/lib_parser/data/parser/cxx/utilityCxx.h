#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <memory>
#include <string>
#include <vector>

#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclTemplate.h"

class DataType;
class NameHierarchy;

namespace utility
{
	template <typename TypeLocType>
	TypeLocType getNextTypeLoc(clang::TypeLoc loc);

	std::shared_ptr<DataType> qualTypeToDataType(clang::QualType qualType);

	NameHierarchy getDeclNameHierarchy(const clang::Decl* declaration);
	std::shared_ptr<DataType> templateArgumentToDataType(const clang::TemplateArgument& argument);
	NameHierarchy getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration);
}

template <typename TypeLocType>
TypeLocType utility::getNextTypeLoc(clang::TypeLoc loc)
{
	while (!loc.isNull())
	{
		TypeLocType ret = loc.getAs<TypeLocType>();
		if (!ret.isNull())
		{
			return ret;
		}
		loc = loc.getNextTypeLoc();
	}
	return TypeLocType(); // isNull() of this object returns true.
}

#endif // UTILITY_CLANG_H
