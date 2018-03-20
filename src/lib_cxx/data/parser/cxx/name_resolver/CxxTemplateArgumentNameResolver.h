#ifndef CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H
#define CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H

#include <memory>

#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class DataType;

class CxxTemplateArgumentNameResolver: public CxxNameResolver
{
public:
	CxxTemplateArgumentNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
	CxxTemplateArgumentNameResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);

	std::wstring getTemplateArgumentName(const clang::TemplateArgument& argument);
};

#endif // CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H
