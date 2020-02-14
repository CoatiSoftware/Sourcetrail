#include "CxxTemplateParameterStringResolver.h"

#include <sstream>

#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>

#include "CxxTypeNameResolver.h"
#include "utilityString.h"

CxxTemplateParameterStringResolver::CxxTemplateParameterStringResolver(
	CanonicalFilePathCache* canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache)
{
}

CxxTemplateParameterStringResolver::CxxTemplateParameterStringResolver(const CxxNameResolver* other)
	: CxxNameResolver(other)
{
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterString(
	const clang::NamedDecl* parameter)
{
	std::wstring templateParameterTypeString;

	if (parameter)
	{
		const clang::Decl::Kind templateParameterKind = parameter->getKind();
		switch (templateParameterKind)
		{
		case clang::Decl::NonTypeTemplateParm:
			templateParameterTypeString = getTemplateParameterTypeString(
				clang::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTypeParm:
			templateParameterTypeString = getTemplateParameterTypeString(
				clang::dyn_cast<clang::TemplateTypeParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTemplateParm:
			templateParameterTypeString = getTemplateParameterTypeString(
				clang::dyn_cast<clang::TemplateTemplateParmDecl>(parameter));
			break;
		default:
			// LOG_ERROR("Unhandled kind of template parameter.");
			break;
		}

		const std::wstring parameterName = utility::decodeFromUtf8(parameter->getName());
		if (!parameterName.empty())
		{
			templateParameterTypeString += L' ' + parameterName;
		}
	}
	return templateParameterTypeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(
	const clang::NonTypeTemplateParmDecl* parameter)
{
	std::wstring typeString = CxxTypeName::makeUnsolvedIfNull(
								  CxxTypeNameResolver(this).getName(parameter->getType()))
								  ->toString();

	if (parameter->isTemplateParameterPack())
	{
		typeString += L"...";
	}

	return typeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(
	const clang::TemplateTypeParmDecl* parameter)
{
	std::wstring typeString = (parameter->wasDeclaredWithTypename() ? L"typename" : L"class");

	if (parameter->isTemplateParameterPack())
	{
		typeString += L"...";
	}

	return typeString;
}

std::wstring CxxTemplateParameterStringResolver::getTemplateParameterTypeString(
	const clang::TemplateTemplateParmDecl* parameter)
{
	std::wstringstream ss;
	ss << L"template<";
	const clang::TemplateParameterList* parameterList = parameter->getTemplateParameters();
	for (unsigned i = 0; i < parameterList->size(); i++)
	{
		if (i > 0)
		{
			ss << L", ";
		}

		CxxTemplateParameterStringResolver parameterStringResolver(this);
		parameterStringResolver.ignoreContextDecl(parameterList->getParam(i));

		ss << parameterStringResolver.getTemplateParameterString(parameterList->getParam(i));
	}
	ss << L"> typename";	// TODO: what if template template parameter is defined with class keyword?

	if (parameter->isTemplateParameterPack())
	{
		ss << L"...";
	}

	return ss.str();
}
