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

std::shared_ptr<CxxName> CxxSpecifierNameResolver::getName(const clang::NestedNameSpecifier* nestedNameSpecifier)
{
	std::shared_ptr<CxxName> name;
	if (!nestedNameSpecifier)
	{
		return name;
	}

	clang::NestedNameSpecifier::SpecifierKind nnsKind = nestedNameSpecifier->getKind();
	switch (nnsKind)
	{
	case clang::NestedNameSpecifier::Identifier:
		{
			name = std::make_shared<CxxDeclName>(
				nestedNameSpecifier->getAsIdentifier()->getName(), std::vector<std::string>()
			);

			if (const clang::NestedNameSpecifier* prefix = nestedNameSpecifier->getPrefix())
			{
				std::shared_ptr<CxxName> parentName = getName(prefix);
				if (parentName)
				{
					name->setParent(parentName);
				}
			}
		}
		break;
	case clang::NestedNameSpecifier::Namespace:
		{
			CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
			name = declNameResolver.getName(nestedNameSpecifier->getAsNamespace());
		}
		break;
	case clang::NestedNameSpecifier::NamespaceAlias:
		{
			CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
			name = declNameResolver.getName(nestedNameSpecifier->getAsNamespaceAlias());
		}
		break;
	case clang::NestedNameSpecifier::TypeSpec:
	case clang::NestedNameSpecifier::TypeSpecWithTemplate:
		{
			CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());
			name = typeNameResolver.getName(nestedNameSpecifier->getAsType());
		}
		break;
	case clang::NestedNameSpecifier::Global:
		// no context name hierarchy needed.
		break;
	case clang::NestedNameSpecifier::Super:
		{
			CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
			name = declNameResolver.getName(nestedNameSpecifier->getAsRecordDecl());
		}
		break;
	}
	return name;
}
