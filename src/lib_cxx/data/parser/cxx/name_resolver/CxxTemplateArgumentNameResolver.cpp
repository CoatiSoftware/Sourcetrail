#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/utilityString.h"

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache, std::vector<const clang::Decl*>())
{
}

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache, 
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: CxxNameResolver(canonicalFilePathCache, ignoredContextDecls)
{
}

std::wstring CxxTemplateArgumentNameResolver::getTemplateArgumentName(const clang::TemplateArgument& argument)
{
	// This doesn't work correctly if the template argument is dependent.
	// If that's required: build name from depth and index of template arg.
	const clang::TemplateArgument::ArgKind kind = argument.getKind();
	switch (kind)
	{
	case clang::TemplateArgument::Type:
		{
			CxxTypeNameResolver typeNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
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

			return utility::decodeFromUtf8(typeName);
		}
	case clang::TemplateArgument::Pack:
		{
			std::wstring typeName = L"<";
			llvm::ArrayRef<clang::TemplateArgument> pack = argument.getPackAsArray();
			for (size_t i = 0; i < pack.size(); i++)
			{
				typeName += getTemplateArgumentName(pack[i]);
				if (i < pack.size() - 1)
				{
					typeName += L", ";
				}
			}
			typeName += L">";

			return typeName;
		}
	}

	return L"";
}
