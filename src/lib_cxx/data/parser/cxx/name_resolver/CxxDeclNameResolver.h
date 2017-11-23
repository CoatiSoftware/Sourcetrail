#ifndef CXX_DECL_NAME_RESOLVER_H
#define CXX_DECL_NAME_RESOLVER_H

#include "clang/AST/DeclTemplate.h"

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CanonicalFilePathCache;

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
	template <typename T>
	std::vector<std::string> getTemplateParameterStringsOfPatrialSpecialitarion(const T* templateDecl);
	std::string getTemplateParameterString(const clang::NamedDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);

	const clang::NamedDecl* m_currentDecl;
};


template <typename T>
std::vector<std::string> CxxDeclNameResolver::getTemplateParameterStringsOfPatrialSpecialitarion(const T* partialSpecializationDecl)
{
	std::vector<std::string> templateParameterNames;
	clang::TemplateParameterList* parameterList = partialSpecializationDecl->getTemplateParameters();
	unsigned int currentParameterIndex = 0;

	const clang::TemplateArgumentList& templateArgumentList = partialSpecializationDecl->getTemplateArgs();
	const int templateArgumentCount = templateArgumentList.size();
	for (int i = 0; i < templateArgumentCount; i++)
	{
		const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
		if (templateArgument.isDependent()) //  IMPORTANT_TODO: fix case when arg depends on template parameter of outer template class, or depends on first template parameter.
		{
			if (currentParameterIndex < parameterList->size())
			{
				templateParameterNames.push_back(getTemplateParameterString(parameterList->getParam(currentParameterIndex)));
			}
			else
			{
				//this if fixes the crash, but not the problem TODO
				// const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
				// LOG_ERROR("Template getParam out of Range " + declaration->getLocation().printToString(sourceManager));
			}
			currentParameterIndex++;
		}
		else
		{
			templateParameterNames.push_back(getTemplateArgumentName(templateArgument));
		}
	}
	return templateParameterNames;
}

#endif // CXX_DECL_NAME_RESOLVER_H
