#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/type/DataType.h"
#include "data/type/NamedDataType.h"
#include "data/type/ArrayModifiedDataType.h"
#include "data/type/PointerModifiedDataType.h"
#include "data/type/ReferenceModifiedDataType.h"
#include "utility/logging/logging.h"

CxxTypeNameResolver::CxxTypeNameResolver()
	: CxxNameResolver(std::vector<const clang::Decl*>())
{
}

CxxTypeNameResolver::CxxTypeNameResolver(std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
{
}

CxxTypeNameResolver::~CxxTypeNameResolver()
{
}

std::shared_ptr<DataType> CxxTypeNameResolver::qualTypeToDataType(clang::QualType qualType)
{
	std::shared_ptr<DataType> dataType = typeToDataType(qualType.getTypePtr());
	if (qualType.isConstQualified())
	{
		dataType->addQualifier(DataType::QUALIFIER_CONST);
	}
	return dataType;
}

std::shared_ptr<DataType> CxxTypeNameResolver::typeToDataType(const clang::Type* type)
{
	std::shared_ptr<DataType> dataType;

	switch (type->getTypeClass())
	{
	case clang::Type::Paren:
		{
			dataType = qualTypeToDataType(type->getAs<clang::ParenType>()->getInnerType());
			break;
		}
	case clang::Type::Typedef:
		{
			CxxDeclNameResolver declNameResolver(type->getAs<clang::TypedefType>()->getDecl(), getIgnoredContextDecls());
			dataType = std::make_shared<NamedDataType>(declNameResolver.getDeclNameHierarchy());
			break;
		}
	case clang::Type::MemberPointer:
		{
			// test this case!
		}
	case clang::Type::Pointer:
		{
			std::shared_ptr<DataType> innerType = qualTypeToDataType(type->getPointeeType());
			dataType = std::make_shared<PointerModifiedDataType>(innerType);
			break;
		}
	case clang::Type::ConstantArray:
	case clang::Type::VariableArray:
	case clang::Type::DependentSizedArray:
	case clang::Type::IncompleteArray:
		{
			std::shared_ptr<DataType> innerType = qualTypeToDataType(clang::dyn_cast<clang::ArrayType>(type)->getElementType());
			dataType = std::make_shared<ArrayModifiedDataType>(innerType);
			break;
		}
	case clang::Type::LValueReference:
	case clang::Type::RValueReference:
		{
			std::shared_ptr<DataType> innerType = qualTypeToDataType(type->getPointeeType());
			dataType = std::make_shared<ReferenceModifiedDataType>(innerType);
			break;
		}
	case clang::Type::Elaborated:
		{
			dataType = qualTypeToDataType(clang::dyn_cast<clang::ElaboratedType>(type)->getNamedType());
			break;
		}
	case clang::Type::Enum:
	case clang::Type::Record:
		{
			CxxDeclNameResolver declNameResolver(type->getAs<clang::TagType>()->getDecl(), getIgnoredContextDecls());
			dataType = std::make_shared<NamedDataType>(declNameResolver.getDeclNameHierarchy());
			break;
		}
	case clang::Type::Builtin:
		{
			clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
			pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
			pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

			clang::SmallString<64> Buf;
			llvm::raw_svector_ostream StrOS(Buf);
			clang::QualType::print(type, clang::Qualifiers(), StrOS, pp, clang::Twine());
			std::string typeName = StrOS.str();

			NameHierarchy typeNameHerarchy;
			typeNameHerarchy.push(std::make_shared<NameElement>(typeName));

			dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
			break;
		}
	case clang::Type::TemplateSpecialization:
		{
			NameHierarchy typeNameHerarchy;

			const clang::TagType* tagType = type->getAs<clang::TagType>(); // remove this case when NameHierarchy is split into namepart and parameter part
			if (tagType)
			{
				CxxDeclNameResolver declNameResolver(tagType->getDecl(), getIgnoredContextDecls());
				typeNameHerarchy = declNameResolver.getDeclNameHierarchy();
			}
			else // specialization of a template template parameter (no concrete class)
			{
				const clang::TemplateSpecializationType* templateSpecializationType = type->getAs<clang::TemplateSpecializationType>();
				CxxDeclNameResolver declNameResolver(templateSpecializationType->getTemplateName().getAsTemplateDecl(), getIgnoredContextDecls());
				typeNameHerarchy = declNameResolver.getDeclNameHierarchy();

				if (typeNameHerarchy.size() > 0)
				{
					std::string templateArgumentNamePart = "<";
					CxxTemplateArgumentNameResolver resolver(getIgnoredContextDecls());
					for (size_t i = 0; i < templateSpecializationType->getNumArgs(); i++)
					{
						templateArgumentNamePart += resolver.getTemplateArgumentName(templateSpecializationType->getArg(i));
						if (i + 1 < templateSpecializationType->getNumArgs())
							templateArgumentNamePart += ", ";
					}
					templateArgumentNamePart += ">";

					std::string declName = typeNameHerarchy.back()->getFullName();
					declName = declName.substr(0, declName.rfind("<"));	// remove template parameters - TODO: FIX: does not work for A<ajaj<ajsj>>
					declName += templateArgumentNamePart;				// add template arguments
					typeNameHerarchy.pop();
					typeNameHerarchy.push(std::make_shared<NameElement>(declName));
				}
			}
			dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
			break;
		}
	case clang::Type::TemplateTypeParm:
		{
			clang::TemplateTypeParmDecl* templateTypeParmDecl = clang::dyn_cast<clang::TemplateTypeParmType>(type)->getDecl();

			CxxDeclNameResolver declNameResolver(templateTypeParmDecl, getIgnoredContextDecls());
			NameHierarchy typeNameHerarchy = declNameResolver.getDeclNameHierarchy();

			dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
			break;
		}
	case clang::Type::SubstTemplateTypeParm:
		{
			dataType = qualTypeToDataType(type->getAs<clang::SubstTemplateTypeParmType>()->getReplacementType());
			break;
		}
	case clang::Type::DependentName:
		{
			const clang::DependentNameType* dependentNameType = clang::dyn_cast<clang::DependentNameType>(type);

			NameHierarchy typeNameHerarchy = getNameHierarchy(dependentNameType->getQualifier());
			typeNameHerarchy.push(std::make_shared<NameElement>(dependentNameType->getIdentifier()->getName().str()));

			dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
			break;
		}
	case clang::Type::PackExpansion:
		{
			const clang::PackExpansionType* packExpansionType = clang::dyn_cast<clang::PackExpansionType>(type);
			dataType = qualTypeToDataType(packExpansionType->getPattern());
			break;
		}
	default:
		{
			LOG_INFO(std::string("Unhandled kind of type encountered: ") + type->getTypeClassName());
			clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
			pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
			pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

			clang::SmallString<64> Buf;
			llvm::raw_svector_ostream StrOS(Buf);
			clang::QualType::print(type, clang::Qualifiers(), StrOS, pp, clang::Twine());
			std::string typeName = StrOS.str();

			NameHierarchy typeNameHerarchy;
			typeNameHerarchy.push(std::make_shared<NameElement>(typeName));

			dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
			break;
		}
	}
	return dataType;
}

NameHierarchy CxxTypeNameResolver::getNameHierarchy(const clang::NestedNameSpecifier* nestedNameSpecifier)
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
		typeNameHerarchy = typeToDataType(nestedNameSpecifier->getAsType())->getTypeNameHierarchy();
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
