#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

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
	// This doesn't work correctly if the template argument is dependent.
	// If that's required: build name from depth and index of template arg.
	const clang::TemplateArgument::ArgKind kind = argument.getKind();
	switch (kind)
	{
	case clang::TemplateArgument::Type:
		{
			CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());
			std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typeNameResolver.getName(argument.getAsType()));
			return typeName->toString();
		}
	case clang::TemplateArgument::Integral:
	case clang::TemplateArgument::Null:
	case clang::TemplateArgument::Declaration:
	case clang::TemplateArgument::NullPtr:
	case clang::TemplateArgument::Template:
	case clang::TemplateArgument::TemplateExpansion: // handled correctly? template template parameter...
	case clang::TemplateArgument::Expression:
		{
			clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
			pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
			pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

			std::string buf;
			llvm::raw_string_ostream os(buf);
			argument.print(pp, os);
			const std::string typeName = os.str();

			return typeName;
		}
	case clang::TemplateArgument::Pack:
		{
			std::string typeName = "<";
			argument.getPackAsArray();
			llvm::ArrayRef<clang::TemplateArgument> pack = argument.getPackAsArray();
			for (size_t i = 0; i < pack.size(); i++)
			{
				typeName += getTemplateArgumentName(pack[i]);
				if (i < pack.size() - 1)
				{
					typeName += ", ";
				}
			}
			typeName += ">";

			return typeName;
		}
	}

	return "";
}
