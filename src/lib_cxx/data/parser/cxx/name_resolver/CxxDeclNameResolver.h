#ifndef CXX_DECL_NAME_RESOLVER_H
#define CXX_DECL_NAME_RESOLVER_H

#include "data/name/NameHierarchy.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CxxDeclNameResolver: public CxxNameResolver
{
public:
	CxxDeclNameResolver(const clang::Decl* declaration);
	CxxDeclNameResolver(const clang::Decl* declaration, std::vector<const clang::Decl*> ignoredContextDecls);
	virtual ~CxxDeclNameResolver();

	NameHierarchy getDeclNameHierarchy();
	std::shared_ptr<NameElement> getDeclName();

private:
	NameHierarchy getContextNameHierarchy(const clang::DeclContext* declaration);
	NameHierarchy getContextNameHierarchy(const clang::NestedNameSpecifier* specifier);
	std::shared_ptr<NameElement> getDeclName(const clang::NamedDecl* declaration);
	std::shared_ptr<NameElement> getNameForAnonymousSymbol(const std::string& symbolKindName, const clang::PresumedLoc& presumedBegin);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);

	const clang::Decl* m_declaration;
};

#endif // CXX_DECL_NAME_RESOLVER_H
