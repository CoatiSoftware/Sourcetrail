#ifndef CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H
#define CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H

#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

class CxxTemplateArgumentNameResolver: public CxxNameResolver
{
public:
	CxxTemplateArgumentNameResolver();
	CxxTemplateArgumentNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	~CxxTemplateArgumentNameResolver();

	std::string getTemplateArgumentName(const clang::TemplateArgument& argument);
};

#endif // CXX_TEMPLATE_ARGUMENT_NAME_RESOLVER_H
