#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <memory>
#include <string>
#include <vector>

#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclTemplate.h"

class DataType;
class NameHierarchy;

namespace utility
{
	std::shared_ptr<DataType> qualTypeToDataType(clang::QualType qualType);

	NameHierarchy getDeclNameHierarchy(const clang::Decl* declaration);
	std::shared_ptr<DataType> templateArgumentToDataType(const clang::TemplateArgument& argument);
	NameHierarchy getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration);
}

#endif // UTILITY_CLANG_H
