#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <memory>
#include <string>
#include <vector>

#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclTemplate.h"

class DataType;

namespace utility
{
	DataType qualTypeToDataType(clang::QualType qualType);

	std::vector<std::string> getDeclNameHierarchy(const clang::Decl* declaration);
	std::vector<std::string> getContextNameHierarchy(const clang::DeclContext* declaration);
	std::string getDeclName(const clang::NamedDecl* declaration);
	std::vector<std::string> getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration);
	DataType templateArgumentToDataType(const clang::TemplateArgument& argument);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);
}

#endif // UTILITY_CLANG_H
