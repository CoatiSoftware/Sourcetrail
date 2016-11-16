#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
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

std::shared_ptr<CxxTypeName> CxxTypeNameResolver::getName(const clang::QualType& qualType)
{
	std::shared_ptr<CxxTypeName> typeName = getName(qualType.getTypePtr());
	if (qualType.isConstQualified())
	{
		typeName->addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
	}
	return typeName;
}

std::shared_ptr<CxxTypeName> CxxTypeNameResolver::getName(const clang::Type* type)
{
	std::shared_ptr<CxxTypeName> typeName;

	switch (type->getTypeClass())
	{
	case clang::Type::Paren:
	{
		typeName = getName(type->getAs<clang::ParenType>()->getInnerType());
		break;
	}
	case clang::Type::Typedef:
	{
		CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
		std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(type->getAs<clang::TypedefType>()->getDecl());

		typeName = std::make_shared<CxxTypeName>(
			declName->getName(),
			std::vector<std::string>(),
			declName->getParent()
		);
		break;
	}
	case clang::Type::MemberPointer:
	{
		// test this case!
	}
	case clang::Type::Pointer:
	{
		typeName = getName(type->getPointeeType());
		typeName->addModifier(CxxTypeName::Modifier("*"));
		break;
	}
	case clang::Type::ConstantArray:
	case clang::Type::DependentSizedArray:
	case clang::Type::IncompleteArray:
	case clang::Type::VariableArray:
	{
		typeName = getName(clang::dyn_cast<clang::ArrayType>(type)->getElementType());
		typeName->addModifier(CxxTypeName::Modifier("[]"));
		break;
	}
	case clang::Type::LValueReference:
	case clang::Type::RValueReference:
	{
		typeName = getName(type->getPointeeType());
		typeName->addModifier(CxxTypeName::Modifier("&"));
		break;
	}
	case clang::Type::Elaborated:
	{
		typeName = getName(clang::dyn_cast<clang::ElaboratedType>(type)->getNamedType());
		break;
	}
	case clang::Type::Enum:
	case clang::Type::Record:
	{
		CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
		std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(type->getAs<clang::TagType>()->getDecl());

		typeName = std::make_shared<CxxTypeName>(
			declName->getName(),
			std::vector<std::string>(),
			declName->getParent()
		);
		break;
	}
	case clang::Type::Builtin:
	{
		clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
		pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
		pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

		typeName = std::make_shared<CxxTypeName>(
			type->getAs<clang::BuiltinType>()->getName(pp), std::vector<std::string>()
		);
		break;
	}
	case clang::Type::TemplateSpecialization:
	{
		const clang::TagType* tagType = type->getAs<clang::TagType>(); // remove this case when NameHierarchy is split into namepart and parameter part
		if (tagType)
		{
			CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
			std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(tagType->getDecl());

			typeName = std::make_shared<CxxTypeName>(
				declName->getName(),
				declName->getTemplateParameterNames(),
				declName->getParent()
			);
		}
		else // specialization of a template template parameter (no concrete class) important, may help: has no underlying decl!
		{
			const clang::TemplateSpecializationType* templateSpecializationType = type->getAs<clang::TemplateSpecializationType>();
			CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
			const std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(templateSpecializationType->getTemplateName().getAsTemplateDecl());

			if (declName)
			{
				std::vector<std::string> templateArguments;
				CxxTemplateArgumentNameResolver resolver(getIgnoredContextDecls());
				for (size_t i = 0; i < templateSpecializationType->getNumArgs(); i++)
				{
					templateArguments.push_back(resolver.getTemplateArgumentName(templateSpecializationType->getArg(i)));
				}

				typeName = std::make_shared<CxxTypeName>(
					declName->getName(), templateArguments, declName->getParent()
				);
			}
			else
			{
				LOG_WARNING("no decl found");
			}
		}
		break;
	}
	case clang::Type::TemplateTypeParm:
	{
		CxxDeclNameResolver declNameResolver(getIgnoredContextDecls());
		std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(clang::dyn_cast<clang::TemplateTypeParmType>(type)->getDecl());

		typeName = std::make_shared<CxxTypeName>(
			declName->getName(),
			declName->getTemplateParameterNames(),
			declName->getParent()
		);
		break;
	}
	case clang::Type::SubstTemplateTypeParm:
	{
		typeName = getName(type->getAs<clang::SubstTemplateTypeParmType>()->getReplacementType());
		break;
	}
	case clang::Type::DependentName:
	{
		const clang::DependentNameType* dependentNameType = clang::dyn_cast<clang::DependentNameType>(type);

		CxxSpecifierNameResolver specifierNameResolver(getIgnoredContextDecls());
		std::shared_ptr<CxxName> specifierName = specifierNameResolver.getName(dependentNameType->getQualifier());

		// TODO: TEst what if this one has template args?
		typeName = std::make_shared<CxxTypeName>(
			dependentNameType->getIdentifier()->getName().str(), std::vector<std::string>(), specifierName
		);
		break;
	}
	case clang::Type::PackExpansion:
	{
		typeName = getName(clang::dyn_cast<clang::PackExpansionType>(type)->getPattern());
		break;
	}
	case clang::Type::Auto:
	{
		clang::QualType deducedType = clang::dyn_cast<clang::AutoType>(type)->getDeducedType();
		if (!deducedType.isNull())
		{
			typeName = getName(deducedType);
		}
		else
		{
			typeName = std::make_shared<CxxTypeName>(
				"auto", std::vector<std::string>()
			);
		}
		break;
	}
	case clang::Type::Decltype:
	{
		typeName = getName(clang::dyn_cast<clang::DecltypeType>(type)->getUnderlyingType());
		break;
	}
	default:
	{
		std::string typeClassName = type->getTypeClassName();
		LOG_INFO(std::string("Unhandled kind of type encountered: ") + typeClassName);
		clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
		pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
		pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

		clang::SmallString<64> Buf;
		llvm::raw_svector_ostream StrOS(Buf);
		clang::QualType::print(type, clang::Qualifiers(), StrOS, pp, clang::Twine());
		std::string nameString = StrOS.str();

		typeName = std::make_shared<CxxTypeName>(
			nameString, std::vector<std::string>()
		);
		break;
	}
	}
	return typeName;
}
