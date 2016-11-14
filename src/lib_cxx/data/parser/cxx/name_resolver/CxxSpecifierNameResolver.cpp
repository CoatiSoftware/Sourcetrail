#include "data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"

CxxSpecifierNameResolver::CxxSpecifierNameResolver()
	: CxxNameResolver(std::vector<const clang::Decl*>())
{
}

CxxSpecifierNameResolver::CxxSpecifierNameResolver(std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
{
}

CxxSpecifierNameResolver::~CxxSpecifierNameResolver()
{
}

NameHierarchy CxxSpecifierNameResolver::getNameHierarchy(const clang::NestedNameSpecifier* nestedNameSpecifier)
{
	clang::NestedNameSpecifier::SpecifierKind nnsKind = nestedNameSpecifier->getKind();
	NameHierarchy typeNameHerarchy;
	switch (nnsKind)
	{
	case clang::NestedNameSpecifier::Identifier:
		{
			const clang::NestedNameSpecifier* prefix = nestedNameSpecifier->getPrefix();
			if (prefix)
			{
				typeNameHerarchy = getNameHierarchy(prefix);
			}
			typeNameHerarchy.push(std::make_shared<NameElement>(nestedNameSpecifier->getAsIdentifier()->getName()));
		}
		break;
	case clang::NestedNameSpecifier::Namespace:
		{
			CxxDeclNameResolver declNameResolver(nestedNameSpecifier->getAsNamespace(), getIgnoredContextDecls());
			typeNameHerarchy = declNameResolver.getDeclNameHierarchy();
		}
		break;
	case clang::NestedNameSpecifier::NamespaceAlias:
		{
			CxxDeclNameResolver declNameResolver(nestedNameSpecifier->getAsNamespaceAlias(), getIgnoredContextDecls());
			typeNameHerarchy = declNameResolver.getDeclNameHierarchy();
		}
		break;
	case clang::NestedNameSpecifier::TypeSpec:
	case clang::NestedNameSpecifier::TypeSpecWithTemplate:
		{
			CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());
			typeNameHerarchy = typeNameResolver.getTypeNameHierarchy(nestedNameSpecifier->getAsType());
		}
		break;
	case clang::NestedNameSpecifier::Global:
		// no context name hierarchy needed.
		break;
	case clang::NestedNameSpecifier::Super:
		{
			CxxDeclNameResolver declNameResolver(nestedNameSpecifier->getAsRecordDecl(), getIgnoredContextDecls());
			typeNameHerarchy = declNameResolver.getDeclNameHierarchy();
		}
		break;
	}
	return typeNameHerarchy;
}
