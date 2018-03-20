#include "data/parser/cxx/name_resolver/CxxTemplateParameterStringResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/utilityString.h"

CxxTemplateParameterStringResolver::CxxTemplateParameterStringResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache, std::vector<const clang::Decl*>())
{
}

CxxTemplateParameterStringResolver::CxxTemplateParameterStringResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache, 
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: CxxNameResolver(canonicalFilePathCache, ignoredContextDecls)
{
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterString(const clang::NamedDecl* parameter)
{
	std::wstring templateParameterTypeString;

	if (parameter)
	{
		const std::wstring parameterName = utility::decodeFromUtf8(parameter->getName());

		const clang::Decl::Kind templateParameterKind = parameter->getKind();
		switch (templateParameterKind)
		{
		case clang::Decl::NonTypeTemplateParm:
			templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTypeParm:
			templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTypeParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTemplateParm:
			templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTemplateParmDecl>(parameter));
			break;
		default:
			// LOG_ERROR("Unhandled kind of template parameter.");
			break;
		}

		if (!parameterName.empty())
		{
			templateParameterTypeString += L" " + parameterName;
		}
	}
	return templateParameterTypeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter)
{
	CxxTypeNameResolver typeNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
	std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typeNameResolver.getName(parameter->getType()));

	std::wstring typeString = typeName->toString();

	if (parameter->isTemplateParameterPack())
	{
		typeString += L"...";
	}

	return typeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter)
{
	std::wstring typeString = (parameter->wasDeclaredWithTypename() ? L"typename" : L"class");

	if (parameter->isTemplateParameterPack())
	{
		typeString += L"...";
	}

	return typeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter)
{
	std::wstring templateParameterTypeString = L"template<";
	clang::TemplateParameterList* parameterList = parameter->getTemplateParameters();
	for (size_t i = 0; i < parameterList->size(); i++)
	{
		CxxTemplateParameterStringResolver parameterStringResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
		parameterStringResolver.ignoreContextDecl(parameterList->getParam(i));

		templateParameterTypeString += parameterStringResolver.getTemplateParameterString(parameterList->getParam(i));
		templateParameterTypeString += (i < parameterList->size() - 1) ? L", " : L"";
	}
	templateParameterTypeString += L">";
	templateParameterTypeString += L" typename"; // TODO: what if template template parameter is defined with class keyword?

	if (parameter->isTemplateParameterPack())
	{
		templateParameterTypeString += L"...";
	}

	return templateParameterTypeString;
}
