#ifndef CXX_DECL_NAME_RESOLVER_H
#define CXX_DECL_NAME_RESOLVER_H

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CanonicalFilePathCache;
class FilePath;

class CxxDeclNameResolver: public CxxNameResolver
{
public:
	CxxDeclNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
	CxxDeclNameResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);
	virtual ~CxxDeclNameResolver();

	std::shared_ptr<CxxDeclName> getName(const clang::NamedDecl* declaration);

private:
	std::shared_ptr<CxxName> getContextName(const clang::DeclContext* declaration);
	std::shared_ptr<CxxDeclName> getDeclName(const clang::NamedDecl* declaration);
	std::string getTranslationUnitMainFileName(const clang::Decl* declaration);
	std::string getDeclarationFileName(const clang::Decl* declaration);
	std::string getNameForAnonymousSymbol(const std::string& symbolKindName, const clang::Decl* declaration);
	std::vector<std::string> getTemplateParameterStrings(const clang::TemplateDecl* templateDecl);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);

	const clang::NamedDecl* m_currentDecl;
};

#endif // CXX_DECL_NAME_RESOLVER_H
