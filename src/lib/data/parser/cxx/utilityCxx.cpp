#include "data/parser/cxx/utilityCxx.h"

#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/type/DataType.h"
#include "data/type/NamedDataType.h"
#include "data/type/ArrayModifiedDataType.h"
#include "data/type/PointerModifiedDataType.h"
#include "data/type/ReferenceModifiedDataType.h"

#include "utility/utilityString.h"
#include "utility/logging/logging.h"

namespace utility
{
	std::shared_ptr<DataType> qualTypeToDataType(clang::QualType qualType)
	{
		std::shared_ptr<DataType> dataType = typeToDataType(qualType.getTypePtr());
		if (qualType.isConstQualified())
		{
			dataType->addQualifier(DataType::QUALIFIER_CONST);
		}
		return dataType;
	}

	std::shared_ptr<DataType> typeToDataType(const clang::Type* type)
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
				dataType = std::make_shared<NamedDataType>(getDeclNameHierarchy(type->getAs<clang::TypedefType>()->getDecl()));
				break;
			}
		case clang::Type::MemberPointer:
			{
				int ogogo = 0; // test this case!
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
				dataType = std::make_shared<NamedDataType>(getDeclNameHierarchy(type->getAs<clang::TagType>()->getDecl()));
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
					typeNameHerarchy = getDeclNameHierarchy(tagType->getDecl());
				}
				else // specialization depends on template template parameter type
				{
					const clang::TemplateSpecializationType* templateSpecializationType = type->getAs<clang::TemplateSpecializationType>();
					typeNameHerarchy = getDeclNameHierarchy(templateSpecializationType->getTemplateName().getAsTemplateDecl());

					if (typeNameHerarchy.size() > 0)
					{
						std::string templateArgumentPart = "<";
						for (int i = 0; i < templateSpecializationType->getNumArgs(); i++)
						{
							templateArgumentPart += getTemplateArgumentName(templateSpecializationType->getArg(i));
							if (i < templateSpecializationType->getNumArgs() - 1)
								templateArgumentPart += ", ";
						}
						templateArgumentPart += ">";

						std::string declName = typeNameHerarchy.back()->getFullName();
						declName = declName.substr(0, declName.rfind("<"));	// remove template parameters - does not work for A<ajaj<ajsj>>
						declName += templateArgumentPart;					// add template arguments
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

				std::string typeName = getDeclName(templateTypeParmDecl);
				NameHierarchy typeNameHerarchy = getContextNameHierarchyOfTemplateParameter(templateTypeParmDecl);

				if (typeNameHerarchy.size() == 0)
				{
					LOG_ERROR("Unable to resolve type name hierarchy for template parameter \"" + typeName + "\"");
					typeNameHerarchy.push(std::make_shared<NameElement>(typeName));
				}
				else
				{
					std::string lastContextElementName = typeNameHerarchy.back()->getFullName();
					typeNameHerarchy.pop();
					typeNameHerarchy.push(std::make_shared<NameElement>(lastContextElementName + "::" + typeName));
				}
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
				clang::NestedNameSpecifier* nns = dependentNameType->getQualifier();
				clang::NestedNameSpecifier::SpecifierKind nnsKind = nns->getKind();
				NameHierarchy typeNameHerarchy;
				switch (nnsKind)
				{
				case clang::NestedNameSpecifier::Identifier:
					typeNameHerarchy.push(std::make_shared<NameElement>(nns->getAsIdentifier()->getName()));
					LOG_ERROR("Unable to resolve name of nested name specifier of kind: Identifier"); // this one is not tested yet. tell malte if you get this log error.
					break;
				case clang::NestedNameSpecifier::Namespace:
					typeNameHerarchy = getDeclNameHierarchy(nns->getAsNamespace());
					break;
				case clang::NestedNameSpecifier::NamespaceAlias:
					typeNameHerarchy = getDeclNameHierarchy(nns->getAsNamespaceAlias());
					break;
				case clang::NestedNameSpecifier::TypeSpec:
				case clang::NestedNameSpecifier::TypeSpecWithTemplate:
					typeNameHerarchy = typeToDataType(nns->getAsType())->getTypeNameHierarchy();
					break;
				case clang::NestedNameSpecifier::Global:
					// no context name hierarchy needed.
					break;
				case clang::NestedNameSpecifier::Super:
					typeNameHerarchy = getDeclNameHierarchy(nns->getAsRecordDecl());
					break;
				}

				typeNameHerarchy.push(std::make_shared<NameElement>(dependentNameType->getIdentifier()->getName().str()));

				dataType = std::make_shared<NamedDataType>(typeNameHerarchy);
				break;
			}
		default:
			{
				LOG_ERROR(std::string("Unhandled kind of type encountered: ") + type->getTypeClassName());
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

	NameHierarchy getDeclNameHierarchy(const clang::Decl* declaration)
	{
		NameHierarchy contextNameHierarchy;
		if (declaration)
		{
			std::string declName = "";

			if (clang::isa<clang::NamedDecl>(declaration))
			{
				declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(declaration));
			}
			else
			{
				LOG_ERROR("unhandled declaration type: " + std::string(declaration->getDeclKindName()));
			}
			contextNameHierarchy = getContextNameHierarchy(declaration->getDeclContext());
			if (clang::isa<clang::NonTypeTemplateParmDecl>(declaration) ||
				clang::isa<clang::TemplateTypeParmDecl>(declaration) ||
				clang::isa<clang::TemplateTemplateParmDecl>(declaration))
			{
				std::string lastContextElementName = contextNameHierarchy.back()->getFullName();
				contextNameHierarchy.pop();
				contextNameHierarchy.push(std::make_shared<NameElement>(lastContextElementName + "::" + declName));

			}
			else
			{
				contextNameHierarchy.push(std::make_shared<NameElement>(declName));
			}
		}
		return contextNameHierarchy;
	}

	NameHierarchy getContextNameHierarchy(const clang::DeclContext* declContext)
	{
		NameHierarchy contextNameHierarchy;

		const clang::DeclContext* parentContext = declContext->getParent();
		if (parentContext)
		{
			contextNameHierarchy = getContextNameHierarchy(parentContext);
		}

		if (clang::isa<clang::NamedDecl>(declContext))
		{
			std::string declName = getDeclName(clang::dyn_cast<clang::NamedDecl>(declContext));
			if (declName != "")
			{
				contextNameHierarchy.push(std::make_shared<NameElement>(declName));
			}
		}
		return contextNameHierarchy;
	}

	NameHierarchy getContextNameHierarchyOfTemplateParameter(const clang::NamedDecl* templateParmDecl) // why do we need this??
	{
		NameHierarchy contextNameHierarchy;

		const clang::Decl* parentNode = getAstParentDecl(templateParmDecl);
		if (parentNode && clang::isa<clang::NamedDecl>(parentNode))
		{
			const clang::NamedDecl* parentNamedDecl = clang::dyn_cast<clang::NamedDecl>(parentNode);
			contextNameHierarchy = getDeclNameHierarchy(parentNamedDecl);
		}

		return contextNameHierarchy;
	}

	std::string getDeclName(const clang::NamedDecl* declaration)
	{
		std::string declName = declaration->getNameAsString();
		clang::Decl::Kind kind = declaration->getKind();
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

				clang::TemplateParameterList* parameterList = partialSpecializationDecl->getTemplateParameters();
				int currentParameterIndex = 0;

				std::string specializedParameterNamePart = "<";
				int templateArgumentCount = partialSpecializationDecl->getTemplateArgs().size();
				const clang::TemplateArgumentList& templateArgumentList = partialSpecializationDecl->getTemplateArgs();
				for (int i = 0; i < templateArgumentCount; i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
					if (templateArgument.isDependent()) // TODO: fix case when arg depends on template parameter of outer template class.
					{
						specializedParameterNamePart += getTemplateParameterString(parameterList->getParam(currentParameterIndex));
						currentParameterIndex++;
					}
					else
					{
						specializedParameterNamePart += getTemplateArgumentName(templateArgument);
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
					specializedParameterNamePart += getTemplateArgumentName(templateArgumentList.get(i));
					specializedParameterNamePart += (i < templateArgumentList.size() - 1) ? ", " : "";
				}
				specializedParameterNamePart += ">";
				declName += specializedParameterNamePart;
			}
		}
		else if (clang::isa<clang::FunctionDecl>(declaration))
		{
			clang::FunctionTemplateDecl* templateFunctionDeclaration = clang::dyn_cast<clang::FunctionDecl>(declaration)->getDescribedFunctionTemplate();
			if (templateFunctionDeclaration)
			{
				declName = getDeclName(templateFunctionDeclaration);
			}
			else if (clang::dyn_cast<clang::FunctionDecl>(declaration)->isFunctionTemplateSpecialization())
			{
				std::string specializedParameterNamePart = "<";
				const clang::TemplateArgumentList* templateArgumentList = clang::dyn_cast<clang::FunctionDecl>(declaration)->getTemplateSpecializationArgs();
				for (size_t i = 0; i < templateArgumentList->size(); i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
					specializedParameterNamePart += templateArgumentToDataType(templateArgument)->getFullTypeName();
					specializedParameterNamePart += (i < templateArgumentList->size() - 1) ? ", " : "";
				}
				specializedParameterNamePart += ">";
				declName += specializedParameterNamePart;
			}
		}
		else if (clang::isa<clang::TemplateDecl>(declaration)) // also triggers on TemplateTemplateParmDecl
		{
			std::string templateParameterNamePart = "<";
			clang::TemplateParameterList* parameterList = clang::dyn_cast<clang::TemplateDecl>(declaration)->getTemplateParameters();
			for (size_t i = 0; i < parameterList->size(); i++)
			{
				templateParameterNamePart += getTemplateParameterString(parameterList->getParam(i));
				templateParameterNamePart += (i < parameterList->size() - 1) ? ", " : "";
			}
			templateParameterNamePart += ">";
			declName += templateParameterNamePart;
		}
		else if (clang::isa<clang::NamespaceDecl>(declaration) && clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
			declName = "anonymous namespace (" + std::string(presumedBegin.getFilename()) + ")";
		}
		return declName;
	}

	NameHierarchy getTemplateSpecializationParentNameHierarchy(clang::ClassTemplateSpecializationDecl* declaration)
	{
		NameHierarchy specializationParentNameHierarchy;
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

	std::shared_ptr<DataType> templateArgumentToDataType(const clang::TemplateArgument& argument) // remove this! this is stupid! agurment is not always a datatype.
	{
		const clang::TemplateArgument::ArgKind kind = argument.getKind();
		switch (kind)
		{
		case clang::TemplateArgument::Type:
			return utility::qualTypeToDataType(argument.getAsType());
		case clang::TemplateArgument::Integral:
			return utility::qualTypeToDataType(argument.getIntegralType());
		case clang::TemplateArgument::Null:
			LOG_ERROR("Type of template argument not handled: Null");
			break;
		case clang::TemplateArgument::Declaration:
			return utility::qualTypeToDataType(argument.getAsDecl()->getType());
		case clang::TemplateArgument::NullPtr:
			return utility::qualTypeToDataType(argument.getNullPtrType());
			break;
		case clang::TemplateArgument::Template:
			{
				clang::TemplateName templateName = argument.getAsTemplate();
				clang::TemplateName::NameKind::Template;
				switch (templateName.getKind())
				{
				case clang::TemplateName::Template:
					return std::make_shared<NamedDataType>(getDeclNameHierarchy(templateName.getAsTemplateDecl()));
					break;
				default:
					LOG_ERROR("Type of template argument not handled: Template");
				}
			}
			break;
		case clang::TemplateArgument::TemplateExpansion:
			LOG_ERROR("Type of template argument not handled: TemplateExpansion");
			break;
		case clang::TemplateArgument::Expression:
			return utility::qualTypeToDataType(argument.getAsExpr()->getType());
		case clang::TemplateArgument::Pack:
			LOG_ERROR("Type of template argument not handled: Pack");
			break;
		default:
			LOG_ERROR("Type of template argument not handled." + argument.getKind());
			break;
		}
		return std::make_shared<NamedDataType>(NameHierarchy());
	}

	std::string getTemplateParameterString(const clang::NamedDecl* parameter)
	{
		std::string templateParameterString = "";

		clang::Decl::Kind templateParameterKind = parameter->getKind();
		switch (templateParameterKind)
		{
		case clang::Decl::NonTypeTemplateParm:
			templateParameterString = getTemplateParameterTypeString(clang::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTypeParm:
			templateParameterString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTypeParmDecl>(parameter));
			break;
		case clang::Decl::TemplateTemplateParm:
			templateParameterString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTemplateParmDecl>(parameter));
			break;
		default:
			LOG_ERROR("Unhandled kind of template parameter.");
		}

		std::string parameterName = parameter->getName();
		if (!parameterName.empty())
		{
			templateParameterString += " " + parameterName;
		}
		return templateParameterString;
	}

	std::string getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter)
	{
		std::string templateParameterTypeString = "";

		const clang::QualType parmeterType = parameter->getType();
		clang::Type::TypeClass parmeterTypeClass = parmeterType->getTypeClass();
		if (parmeterTypeClass == clang::Type::TemplateTypeParm)
		{
			clang::TemplateTypeParmDecl* parmeterTypeDecl = clang::dyn_cast<clang::TemplateTypeParmType>(parmeterType)->getDecl();
			if (haveAstAncestorRelation(getAstParentDecl(parameter), parmeterTypeDecl))
			{
				templateParameterTypeString = getDeclName(parmeterTypeDecl);
			}
		}
		else if (parmeterTypeClass == clang::Type::DependentName)
		{
			clang::NestedNameSpecifier* nns = clang::dyn_cast<clang::DependentNameType>(parmeterType)->getQualifier();
			clang::NamedDecl* typeParent;
			switch (nns->getKind())
			{
			case clang::NestedNameSpecifier::Identifier:
				{
					clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
					pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
					pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"
					templateParameterTypeString = parmeterType.getAsString(pp);
				}
				break;
			case clang::NestedNameSpecifier::Namespace:
				typeParent = nns->getAsNamespace();
				break;
			case clang::NestedNameSpecifier::NamespaceAlias:
				typeParent = nns->getAsNamespaceAlias();
				break;
			case clang::NestedNameSpecifier::TypeSpec:
			case clang::NestedNameSpecifier::TypeSpecWithTemplate:
				{
					const clang::Type* ssswd = nns->getAsType();
					clang::Type::TypeClass szz = ssswd->getTypeClass();
					switch (ssswd->getTypeClass())
					{
					case clang::Type::TemplateTypeParm:
						typeParent = clang::dyn_cast<clang::TemplateTypeParmType>(ssswd)->getDecl();
						break;
					default:
						typeParent = nullptr;
						LOG_ERROR("aahhh");
					}
				}
				break;
			case clang::NestedNameSpecifier::Global:
				typeParent = nullptr;
				break;
			case clang::NestedNameSpecifier::Super:
				typeParent = nns->getAsRecordDecl();
				break;
			}
			if (typeParent && haveAstAncestorRelation(getAstParentDecl(parameter), typeParent))
			{
				templateParameterTypeString = getDeclName(typeParent);
				templateParameterTypeString += "::" + clang::dyn_cast<clang::DependentNameType>(parmeterType)->getIdentifier()->getName().str();
			}
		}

		if (templateParameterTypeString.empty()) // this is the default case
		{
			templateParameterTypeString = qualTypeToDataType(parmeterType)->getFullTypeName();
		}
		return templateParameterTypeString;
	}

	std::string getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter)
	{
		return (parameter->wasDeclaredWithTypename() ? "typename" : "class");
	}

	std::string getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter)
	{
		std::string templateParameterTypeString = "template<";
		clang::TemplateParameterList* parameterList = parameter->getTemplateParameters();
		for (size_t i = 0; i < parameterList->size(); i++)
		{
			templateParameterTypeString += getTemplateParameterString(parameterList->getParam(i));
			templateParameterTypeString += (i < parameterList->size() - 1) ? ", " : "";
		}
		templateParameterTypeString += ">";
		templateParameterTypeString += " typename"; // TODO: what if template template parameter is defined with class keyword?
		return templateParameterTypeString;
	}

	std::string getTemplateArgumentName(const clang::TemplateArgument& argument)
	{
		const clang::TemplateArgument::ArgKind kind = argument.getKind();
		switch (kind)
		{
		case clang::TemplateArgument::Type:
			return utility::qualTypeToDataType(argument.getAsType())->getFullTypeName();
		case clang::TemplateArgument::Integral:
		case clang::TemplateArgument::Null:
		case clang::TemplateArgument::Declaration:
		case clang::TemplateArgument::NullPtr:
		case clang::TemplateArgument::Template:
		case clang::TemplateArgument::TemplateExpansion:
		case clang::TemplateArgument::Expression:
			{
				clang::PrintingPolicy pp = clang::PrintingPolicy(clang::LangOptions());
				pp.SuppressTagKeyword = true;	// value "true": for a class A it prints "A" instead of "class A"
				pp.Bool = true;					// value "true": prints bool type as "bool" instead of "_Bool"

				std::string buf;
				llvm::raw_string_ostream os(buf);
				argument.print(pp, os);
				return os.str();
			}
		case clang::TemplateArgument::Pack:
			LOG_ERROR("Type of template argument not handled: Pack");
			break;
		default:
			LOG_ERROR("Type of template argument not handled." + argument.getKind());
			break;
		}
		return std::string();
	}

	bool haveAstAncestorRelation(const clang::Decl* parent, const clang::Decl* child)
	{
		if (parent)
		{
			while (child)
			{
				if (parent == child)
				{
					return true;
				}
				child = getAstParentDecl(child);
			}
		}
		return false;
	}

	const clang::Decl* getAstParentDecl(const clang::Decl* decl)
	{
		clang::ASTContext& astContext = decl->getASTContext();
		llvm::ArrayRef<clang::ast_type_traits::DynTypedNode> parents = astContext.getParents<clang::Decl>(*(decl));
		if (parents.size() > 0) // usually this list contains just one parent node.
		{
			return parents[0].get<clang::Decl>(); // use the fist parent node.
		}
		return nullptr;
	}
}


