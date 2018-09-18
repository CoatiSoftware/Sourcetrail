#include "CxxSpecifierNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "CxxTypeNameResolver.h"
#include "CxxDeclNameResolver.h"
#include "utilityString.h"

CxxSpecifierNameResolver::CxxSpecifierNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache, std::vector<const clang::Decl*>())
{
}

CxxSpecifierNameResolver::CxxSpecifierNameResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: CxxNameResolver(canonicalFilePathCache, ignoredContextDecls)
{
}

std::shared_ptr<CxxName> CxxSpecifierNameResolver::getName(const clang::NestedNameSpecifier* nestedNameSpecifier)
{
	std::shared_ptr<CxxName> name;

	if (nestedNameSpecifier)
	{
		clang::NestedNameSpecifier::SpecifierKind nnsKind = nestedNameSpecifier->getKind();
		switch (nnsKind)
		{
		case clang::NestedNameSpecifier::Identifier:
			{
				name = std::make_shared<CxxDeclName>(
					utility::decodeFromUtf8(nestedNameSpecifier->getAsIdentifier()->getName()), std::vector<std::wstring>()
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
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				name = declNameResolver.getName(nestedNameSpecifier->getAsNamespace());
			}
			break;
		case clang::NestedNameSpecifier::NamespaceAlias:
			{
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				name = declNameResolver.getName(nestedNameSpecifier->getAsNamespaceAlias());
			}
			break;
		case clang::NestedNameSpecifier::TypeSpec:
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			{
				CxxTypeNameResolver typeNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				name = CxxTypeName::makeUnsolvedIfNull(typeNameResolver.getName(nestedNameSpecifier->getAsType()));
			}
			break;
		case clang::NestedNameSpecifier::Global:
			// no context name hierarchy needed.
			break;
		case clang::NestedNameSpecifier::Super:
			{
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				name = declNameResolver.getName(nestedNameSpecifier->getAsRecordDecl());
			}
			break;
		}
	}

	return name;
}
