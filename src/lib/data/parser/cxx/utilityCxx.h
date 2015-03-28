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
	std::shared_ptr<DataType> typeToDataType(const clang::Type* type);

	NameHierarchy getDeclNameHierarchy(const clang::Decl* declaration);
	NameHierarchy getContextNameHierarchy(const clang::DeclContext* declaration);
	NameHierarchy getContextNameHierarchyOfTemplateParameter(const clang::NamedDecl* templateParmDecl);
	std::string getDeclName(const clang::NamedDecl* declaration);
	NameHierarchy getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration);
	std::shared_ptr<DataType> templateArgumentToDataType(const clang::TemplateArgument& argument);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);

	bool haveAstAncestorRelation(const clang::Decl* parent, const clang::Decl* child);
	const clang::Decl* getAstParentDecl(const clang::Decl* decl);
}

#endif // UTILITY_CLANG_H
