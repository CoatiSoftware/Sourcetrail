#include "data/parser/cxx/utilityCxx.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/type/DataType.h"
#include "data/type/modifier/DataTypeModifierArray.h"
#include "data/type/modifier/DataTypeModifierPointer.h"
#include "data/type/modifier/DataTypeModifierReference.h"
#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"
#include "utility/utilityString.h"
#include "utility/logging/logging.h"

namespace utility
{
	DataType qualTypeToDataType(clang::QualType qualType)
	{
		std::vector<std::string> typeNameHerarchy;
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;

		while (true)
		{
			const clang::Type* type = qualType.getTypePtr();
			if (type->getAs<clang::TypedefType>())
			{
				typeNameHerarchy.push_back(utility::substrAfter(qualType.getAsString(), ' '));
				break;
			}
			else if (type->isPointerType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierPointer>();
				if (qualType.isConstQualified())
				{
					modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
				}
				modifierStack.push(modifier);

				qualType = type->getPointeeType();
			}
			else if (type->isArrayType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierArray>();
				if (qualType.isConstQualified())
				{
					modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
				}
				modifierStack.push(modifier);

				qualType = clang::dyn_cast<clang::ArrayType>(type)->getElementType();
			}
			else if (type->isReferenceType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierReference>();
				// references can not be const qualified
				modifierStack.push(modifier);

				qualType = type->getPointeeType();
			}
			else
			{
				const clang::Type* type = qualType.getUnqualifiedType().getTypePtr();

				if (clang::isa<clang::ElaboratedType>(type)) // get the real type here
				{
					const clang::ElaboratedType* et = clang::dyn_cast<clang::ElaboratedType>(type);
					type = et->getNamedType().getUnqualifiedType().getTypePtr();
				}

				if (clang::isa<clang::TagType>(type))
				{
					typeNameHerarchy = getDeclNameHierarchy(clang::dyn_cast<clang::TagType>(type)->getDecl());
				}
				else
				{
					clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
					pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
					pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"
					std::string typeName = qualType.getUnqualifiedType().getAsString(pp);

					if (type->isTemplateTypeParmType())
					{
						clang::TemplateTypeParmDecl* templateTypeParmDecl = clang::dyn_cast<clang::TemplateTypeParmType>(qualType.getUnqualifiedType())->getDecl();
						if (templateTypeParmDecl)
						{
							typeNameHerarchy = getContextNameHierarchy(templateTypeParmDecl->getDeclContext());
							typeNameHerarchy.back() += "::" + typeName;
						}
					}
					else
					{
						typeNameHerarchy.push_back(typeName);
					}
				}
				break;
			}
		}

		if (qualType.isConstQualified())
		{
			qualifierList.addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
		}
		return DataType(typeNameHerarchy, qualifierList, modifierStack);
	}

	std::vector<std::string> getDeclNameHierarchy(const clang::Decl* declaration)
	{
		std::vector<std::string> contextNameHierarchy;
		if (declaration)
		{
			std::string declName = "";

			if (clang::isa<clang::NamedDecl>(declaration))
			{
				declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(declaration));
			}
			else
			{
				LOG_ERROR("unhandled declaration type");
			}
			contextNameHierarchy = getContextNameHierarchy(declaration->getDeclContext());
			if (clang::isa<clang::TemplateTypeParmDecl>(declaration))
			{
				contextNameHierarchy.back() += "::" + declName;
			}
			else
			{
				contextNameHierarchy.push_back(declName);
			}
		}
		return contextNameHierarchy;
	}

	std::vector<std::string> getContextNameHierarchy(const clang::DeclContext* declContext)
	{
		std::vector<std::string> contextNameHierarchy;

		const clang::DeclContext* parentContext = declContext->getParent();
		if (parentContext)
		{
			contextNameHierarchy = getContextNameHierarchy(parentContext);
		}

		if (clang::isa<clang::NamedDecl>(declContext))
		{
			std::string declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(declContext));
			if (declName != "")
			{
				contextNameHierarchy.push_back(declName);
			}
		}
		return contextNameHierarchy;
	}

	std::string getDeclName(const clang::NamedDecl* declaration)
	{
		std::string declName = declaration->getNameAsString();

		if (clang::isa<clang::CXXRecordDecl>(declaration))
		{
			clang::ClassTemplateDecl* templateClassDeclaration = clang::dyn_cast<clang::CXXRecordDecl>(declaration)->getDescribedClassTemplate();
			if (templateClassDeclaration)
			{
				declName = getDeclName(templateClassDeclaration);
			}
			else if (clang::isa<clang::ClassTemplatePartialSpecializationDecl>(declaration))
			{
				const clang::ClassTemplatePartialSpecializationDecl* partialSpecializationDecl =
					clang::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration);

				int templateArgumentCount = partialSpecializationDecl->getTemplateArgs().size();
				const clang::ASTTemplateArgumentListInfo* templateArgumentListInfo = partialSpecializationDecl->getTemplateArgsAsWritten();
				std::string specializedParameterNamePart = "<";
				for (int i = 0; i < templateArgumentCount; i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentListInfo->getTemplateArgs()[i].getArgument();
					const clang::TemplateArgument::ArgKind kind = templateArgument.getKind();
					switch (kind)
					{
					case clang::TemplateArgument::Type:
						specializedParameterNamePart += templateArgument.getAsType().getAsString();
						break;
					case clang::TemplateArgument::Integral:
						specializedParameterNamePart += templateArgument.getIntegralType().getAsString();
						break;
					default:
						LOG_ERROR("Type of template argument not handled.");
						break;
					}
					specializedParameterNamePart += (i < templateArgumentCount - 1) ? ", " : "";
				}
				specializedParameterNamePart += ">";
				declName += specializedParameterNamePart;
			}
			else if (clang::isa<clang::ClassTemplateSpecializationDecl>(declaration))
			{
				std::string specializedParameterNamePart = "<";
				const clang::TemplateArgumentList& templateArgumentList = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration)->getTemplateArgs();
				for (size_t i = 0; i < templateArgumentList.size(); i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
					specializedParameterNamePart += templateArgumentToDataType(templateArgument).getFullTypeName();
					specializedParameterNamePart += (i < templateArgumentList.size() - 1) ? ", " : "";
				}
				specializedParameterNamePart += ">";
				declName += specializedParameterNamePart;
			}
		}
		else if (clang::isa<clang::TemplateDecl>(declaration))
		{
			std::string templateParameterNamePart = "<";
			clang::TemplateParameterList* parameterList = clang::dyn_cast<clang::TemplateDecl>(declaration)->getTemplateParameters();
			for (size_t i = 0; i < parameterList->size(); i++)
			{
				clang::NamedDecl* namedDecl = parameterList->getParam(i);

				templateParameterNamePart += namedDecl->getNameAsString();
				templateParameterNamePart += (i < parameterList->size() - 1) ? ", " : "";
			}
			templateParameterNamePart += ">";
			declName += templateParameterNamePart;
		}
		else if (clang::isa<clang::NamespaceDecl>(declaration) && clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
		{
			declName = "(anonymous namespace)";
		}
		return declName;
	}

	std::vector<std::string> getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration)
	{
		std::vector<std::string> specializationParentNameHierarchy;
		llvm::PointerUnion<clang::ClassTemplateDecl*, clang::ClassTemplatePartialSpecializationDecl*> pu = declaration->getSpecializedTemplateOrPartial();
		if (pu.is<clang::ClassTemplateDecl*>())
		{
			clang::ClassTemplateDecl* specializedFromDecl = pu.get<clang::ClassTemplateDecl*>();
			specializationParentNameHierarchy = utility::getDeclNameHierarchy(specializedFromDecl);
		}
		else if (pu.is<clang::ClassTemplatePartialSpecializationDecl*>())
		{
			clang::ClassTemplatePartialSpecializationDecl* specializedFromDecl = pu.get<clang::ClassTemplatePartialSpecializationDecl*>();
			specializationParentNameHierarchy = utility::getDeclNameHierarchy(specializedFromDecl);
		}
		return specializationParentNameHierarchy;
	}

	DataType templateArgumentToDataType(const clang::TemplateArgument& argument)
	{
		const clang::TemplateArgument::ArgKind kind = argument.getKind();
		switch (kind)
		{
		case clang::TemplateArgument::Type:
			return utility::qualTypeToDataType(argument.getAsType());
		case clang::TemplateArgument::Integral:
			return utility::qualTypeToDataType(argument.getIntegralType());
		default:
			LOG_ERROR("Type of template argument not handled.");
			break;
		}
		return DataType(std::vector<std::string>());
	}
}
