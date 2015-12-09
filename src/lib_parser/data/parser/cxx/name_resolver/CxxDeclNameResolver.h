#ifndef CXX_DECL_NAME_RESOLVER_H
#define CXX_DECL_NAME_RESOLVER_H

#include "data/name/NameHierarchy.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CxxDeclNameResolver: public CxxNameResolver
{
public:
	CxxDeclNameResolver(const clang::Decl* declaration);
	CxxDeclNameResolver(const clang::Decl* declaration, std::vector<const clang::Decl*> ignoredContextDecls);
	~CxxDeclNameResolver();

	NameHierarchy getDeclNameHierarchy();
	std::string getDeclName();

private:
	NameHierarchy getContextNameHierarchy(const clang::DeclContext* declaration);
	std::string getDeclName(const clang::NamedDecl* declaration);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);

	const clang::Decl* m_declaration;
};

#endif // CXX_DECL_NAME_RESOLVER_H
