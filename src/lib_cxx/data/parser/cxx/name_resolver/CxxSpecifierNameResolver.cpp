#include "CxxSpecifierNameResolver.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>

#include "CxxDeclNameResolver.h"
#include "CxxTypeNameResolver.h"
#include "utilityString.h"

CxxSpecifierNameResolver::CxxSpecifierNameResolver(CanonicalFilePathCache* canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache)
{
}

CxxSpecifierNameResolver::CxxSpecifierNameResolver(const CxxNameResolver* other)
	: CxxNameResolver(other)
{
}

std::unique_ptr<CxxName> CxxSpecifierNameResolver::getName(
	const clang::NestedNameSpecifier* nestedNameSpecifier)
{
	if (nestedNameSpecifier)
	{
		clang::NestedNameSpecifier::SpecifierKind nnsKind = nestedNameSpecifier->getKind();
		switch (nnsKind)
		{
		case clang::NestedNameSpecifier::Identifier:
		{
			std::unique_ptr<CxxName> name = std::make_unique<CxxDeclName>(
				utility::decodeFromUtf8(nestedNameSpecifier->getAsIdentifier()->getName().str()));

			if (const clang::NestedNameSpecifier* prefix = nestedNameSpecifier->getPrefix())
			{
				std::unique_ptr<CxxName> parentName = getName(prefix);
				if (parentName)
				{
					name->setParent(std::move(parentName));
				}
			}

			return name;
		}

		case clang::NestedNameSpecifier::Namespace:
			return CxxDeclNameResolver(this).getName(nestedNameSpecifier->getAsNamespace());

		case clang::NestedNameSpecifier::NamespaceAlias:
			return CxxDeclNameResolver(this).getName(nestedNameSpecifier->getAsNamespaceAlias());

		case clang::NestedNameSpecifier::TypeSpec:
		case clang::NestedNameSpecifier::TypeSpecWithTemplate:
			return CxxTypeName::makeUnsolvedIfNull(
				CxxTypeNameResolver(this).getName(nestedNameSpecifier->getAsType()));

		case clang::NestedNameSpecifier::Global:
			// no context name hierarchy needed.
			break;

		case clang::NestedNameSpecifier::Super:
			return CxxDeclNameResolver(this).getName(nestedNameSpecifier->getAsRecordDecl());
		}
	}

	return nullptr;
}
