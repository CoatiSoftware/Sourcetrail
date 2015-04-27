#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/logging/logging.h"

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver()
	: CxxNameResolver(std::vector<const clang::Decl*>())
{
}

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver(std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
{
}

CxxTemplateArgumentNameResolver::~CxxTemplateArgumentNameResolver()
{
}

std::string CxxTemplateArgumentNameResolver::getTemplateArgumentName(const clang::TemplateArgument& argument)
{
	const clang::TemplateArgument::ArgKind kind = argument.getKind();
	switch (kind)
	{
	case clang::TemplateArgument::Type:
		{
			CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());
			return typeNameResolver.qualTypeToDataType(argument.getAsType())->getFullTypeName();
		}
	case clang::TemplateArgument::Integral:
	case clang::TemplateArgument::Null:
	case clang::TemplateArgument::Declaration:
	case clang::TemplateArgument::NullPtr:
	case clang::TemplateArgument::Template:
	case clang::TemplateArgument::TemplateExpansion:
	case clang::TemplateArgument::Expression:
		{
			clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
			pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
			pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

			std::string buf;
			llvm::raw_string_ostream os(buf);
			argument.print(pp, os);
			return os.str();
		}
	case clang::TemplateArgument::Pack:
		LOG_ERROR("Type of template argument not handled: Pack");
		break;
	default:
		LOG_ERROR("Type of template argument not handled." + argument.getKind());
		break;
	}
	return std::string();
}
