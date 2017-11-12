#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "utility/logging/logging.h"

CxxTypeNameResolver::CxxTypeNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache, std::vector<const clang::Decl*>())
{
}

CxxTypeNameResolver::CxxTypeNameResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache, 
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: CxxNameResolver(canonicalFilePathCache, ignoredContextDecls)
{
}

CxxTypeNameResolver::~CxxTypeNameResolver()
{
}

std::shared_ptr<CxxTypeName> CxxTypeNameResolver::getName(const clang::QualType& qualType)
{
	std::shared_ptr<CxxTypeName> typeName = getName(qualType.getTypePtr());
	if (typeName && qualType.isConstQualified())
	{
		typeName->addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
	}
	return typeName;
}

std::shared_ptr<CxxTypeName> CxxTypeNameResolver::getName(const clang::Type* type)
{
	std::shared_ptr<CxxTypeName> typeName;

	if (type)
	{
		switch (type->getTypeClass())
		{
		case clang::Type::Paren:
			{
				typeName = getName(type->getAs<clang::ParenType>()->getInnerType());
				break;
			}
		case clang::Type::Attributed:
			{
				typeName = getName(type->getAs<clang::AttributedType>()->getModifiedType());
				break;
			}
		case clang::Type::InjectedClassName:
			{
				typeName = getName(type->getAs<clang::InjectedClassNameType>()->getInjectedSpecializationType());
				break;
			}
		case clang::Type::Typedef:
			{
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(type->getAs<clang::TypedefType>()->getDecl());
				if (declName)
				{
					typeName = std::make_shared<CxxTypeName>(
						declName->getName(),
						std::vector<std::string>(),
						declName->getParent()
					);
				}
				break;
			}
		case clang::Type::MemberPointer:
		case clang::Type::Pointer:
			{
				typeName = getName(type->getPointeeType());
				if (typeName)
				{
					typeName->addModifier(CxxTypeName::Modifier("*"));
				}
				break;
			}
		case clang::Type::ConstantArray:
		case clang::Type::DependentSizedArray:
		case clang::Type::IncompleteArray:
		case clang::Type::VariableArray:
			{
				typeName = getName(clang::dyn_cast<clang::ArrayType>(type)->getElementType());
				if (typeName)
				{
					typeName->addModifier(CxxTypeName::Modifier("[]"));
				}
				break;
			}
		case clang::Type::LValueReference:
			{
				typeName = getName(type->getPointeeType());
				if (typeName)
				{
					typeName->addModifier(CxxTypeName::Modifier("&"));
				}
				break;
			}
		case clang::Type::RValueReference:
			{
				typeName = getName(type->getPointeeType());
				if (typeName)
				{
					typeName->addModifier(CxxTypeName::Modifier("&&"));
				}
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
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(type->getAs<clang::TagType>()->getDecl());
				if (declName)
				{
					typeName = std::make_shared<CxxTypeName>(
						declName->getName(),
						declName->getTemplateParameterNames(), // contains template arguments if decl is a template specialization
						declName->getParent()
					);
				}
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
					CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
					std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(tagType->getDecl());
					if (declName)
					{
						typeName = std::make_shared<CxxTypeName>(
							declName->getName(),
							declName->getTemplateParameterNames(),
							declName->getParent()
						);
					}
				}
				else // specialization of a template template parameter (no concrete class) important, may help: has no underlying decl!
				{
					const clang::TemplateSpecializationType* templateSpecializationType = type->getAs<clang::TemplateSpecializationType>();
					CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
					const std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(templateSpecializationType->getTemplateName().getAsTemplateDecl());

					if (declName)
					{
						std::vector<std::string> templateArguments;
						CxxTemplateArgumentNameResolver resolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
						resolver.ignoreContextDecl(templateSpecializationType->getTemplateName().getAsTemplateDecl()->getTemplatedDecl());
						for (size_t i = 0; i < templateSpecializationType->getNumArgs(); i++)
						{
							templateArguments.push_back(resolver.getTemplateArgumentName(templateSpecializationType->getArg(i)));
						}

						typeName = std::make_shared<CxxTypeName>(
							declName->getName(),
							templateArguments,
							declName->getParent()
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
				CxxDeclNameResolver declNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				std::shared_ptr<CxxDeclName> declName = declNameResolver.getName(clang::dyn_cast<clang::TemplateTypeParmType>(type)->getDecl());
				if (declName)
				{
					typeName = std::make_shared<CxxTypeName>(
						declName->getName(),
						declName->getTemplateParameterNames(),
						declName->getParent()
					);
				}
				break;
			}
		case clang::Type::SubstTemplateTypeParm:
			{
				typeName = getName(type->getAs<clang::SubstTemplateTypeParmType>()->getReplacementType());
				break;
			}
		case clang::Type::DependentName:
			{
				const clang::DependentNameType* dependentType = clang::dyn_cast<clang::DependentNameType>(type);

				CxxSpecifierNameResolver specifierNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				std::shared_ptr<CxxName> specifierName = specifierNameResolver.getName(dependentType->getQualifier());

				typeName = std::make_shared<CxxTypeName>(
					dependentType->getIdentifier()->getName().str(), std::vector<std::string>(), specifierName
				);
				break;
			}
		case clang::Type::DependentTemplateSpecialization:
			{
				const clang::DependentTemplateSpecializationType* dependentType = clang::dyn_cast<clang::DependentTemplateSpecializationType>(type);

				CxxSpecifierNameResolver specifierNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				std::shared_ptr<CxxName> specifierName = specifierNameResolver.getName(dependentType->getQualifier());

				std::vector<std::string> templateArguments;
				CxxTemplateArgumentNameResolver resolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				for (size_t i = 0; i < dependentType->getNumArgs(); i++)
				{
					templateArguments.push_back(resolver.getTemplateArgumentName(dependentType->getArg(i)));
				}

				typeName = std::make_shared<CxxTypeName>(
					dependentType->getIdentifier()->getName().str(), templateArguments, specifierName
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
		case clang::Type::FunctionProto:
			{
				const clang::FunctionProtoType* protoType = clang::dyn_cast<clang::FunctionProtoType>(type);
				std::string nameString = CxxTypeName::makeUnsolvedIfNull(getName(protoType->getReturnType()))->toString();
				nameString += "(";
				for (size_t i = 0; i < protoType->getNumParams(); i++)
				{
					if (i != 0)
					{
						nameString += ", ";
					}
					nameString += CxxTypeName::makeUnsolvedIfNull(getName(protoType->getParamType(i)))->toString();
				}
				nameString += ")";

				typeName = std::make_shared<CxxTypeName>(
					nameString, std::vector<std::string>()
				);
				break;
			}
		case clang::Type::Adjusted:
		case clang::Type::Decayed:
			{
				typeName = getName(type->getAs<clang::AdjustedType>()->getOriginalType());
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
	}
	return typeName;
}
