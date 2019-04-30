#ifndef CXX_DECL_NAME_RESOLVER_H
#define CXX_DECL_NAME_RESOLVER_H

#include <clang/AST/DeclTemplate.h>

#include "CxxDeclName.h"
#include "CxxNameResolver.h"

class CanonicalFilePathCache;

class CxxDeclNameResolver
	: public CxxNameResolver
{
public:
	CxxDeclNameResolver(CanonicalFilePathCache* canonicalFilePathCache);
	CxxDeclNameResolver(const CxxNameResolver* other);

	std::unique_ptr<CxxDeclName> getName(const clang::NamedDecl* declaration);

private:
	std::unique_ptr<CxxName> getContextName(const clang::DeclContext* declaration);
	std::unique_ptr<CxxDeclName> getDeclName(const clang::NamedDecl* declaration);
	std::wstring getTranslationUnitMainFileName(const clang::Decl* declaration);
	std::wstring getNameForAnonymousSymbol(const std::wstring& symbolKindName, const clang::Decl* declaration);
	std::vector<std::wstring> getTemplateParameterStrings(const clang::TemplateDecl* templateDecl);
	template <typename T>
	std::vector<std::wstring> getTemplateParameterStringsOfPartialSpecialization(const T* templateDecl);
	std::wstring getTemplateParameterString(const clang::NamedDecl* parameter);
	std::wstring getTemplateArgumentName(const clang::TemplateArgument& argument);

	const clang::NamedDecl* m_currentDecl;
};


template <typename T>
std::vector<std::wstring> CxxDeclNameResolver::getTemplateParameterStringsOfPartialSpecialization(const T* partialSpecializationDecl)
{
	std::vector<std::wstring> templateParameterNames;
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
