#include "CxxTemplateArgumentNameResolver.h"

#include <sstream>

#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>

#include "CxxTypeNameResolver.h"
#include "utilityString.h"

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver(
	CanonicalFilePathCache* canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache)
{
}

CxxTemplateArgumentNameResolver::CxxTemplateArgumentNameResolver(const CxxNameResolver* other)
	: CxxNameResolver(other)
{
}

std::wstring CxxTemplateArgumentNameResolver::getTemplateArgumentName(
	const clang::TemplateArgument& argument)
{
	// This doesn't work correctly if the template argument is dependent.
	// If that's required: build name from depth and index of template arg.
	const clang::TemplateArgument::ArgKind kind = argument.getKind();
	switch (kind)
	{
	case clang::TemplateArgument::Type:
	{
		CxxTypeNameResolver typeNameResolver(this);
		std::unique_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(
			typeNameResolver.getName(argument.getAsType()));
		return typeName->toString();
	}
	case clang::TemplateArgument::Integral:
	case clang::TemplateArgument::Null:
	case clang::TemplateArgument::Declaration:
	case clang::TemplateArgument::NullPtr:
	case clang::TemplateArgument::Template:
	case clang::TemplateArgument::TemplateExpansion:	// handled correctly? template template parameter...
	case clang::TemplateArgument::Expression:
	{
		clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
		pp.SuppressTagKeyword =
			true;		   // value "true": for a class A it prints "A" instead of "class A"
		pp.Bool = true;	   // value "true": prints bool type as "bool" instead of "_Bool"

		std::string buf;
		llvm::raw_string_ostream os(buf);
		argument.print(pp, os, true);
		return utility::decodeFromUtf8(os.str());
	}
	case clang::TemplateArgument::Pack:
	{
		return L"<...>";
	}
	}

	return L"";
}
