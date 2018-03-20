#ifndef CXX_TEMPLATE_PARAMETER_STRING_RESOLVER_H
#define CXX_TEMPLATE_PARAMETER_STRING_RESOLVER_H

#include <memory>

#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class DataType;

class CxxTemplateParameterStringResolver: public CxxNameResolver
{
public:
	CxxTemplateParameterStringResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
	CxxTemplateParameterStringResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);

	std::wstring getTemplateParameterString(const clang::NamedDecl* parameter);
	std::wstring getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter);
	std::wstring getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter);
	std::wstring getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter);
};

#endif // CXX_TEMPLATE_PARAMETER_STRING_RESOLVER_H
