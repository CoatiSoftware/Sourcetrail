#include "data/parser/cxx/utilityCxx.h"

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "data/type/NamedDataType.h"
#include "utility/logging/logging.h"

namespace utility
{
	std::shared_ptr<DataType> qualTypeToDataType(clang::QualType qualType)
	{
		CxxTypeNameResolver resolver;
		return resolver.qualTypeToDataType(qualType);
	}

	NameHierarchy getDeclNameHierarchy(const clang::Decl* declaration)
	{
		CxxDeclNameResolver resolver(declaration);
		return resolver.getDeclNameHierarchy();
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
		}
		return std::make_shared<NamedDataType>(NameHierarchy());
	}
}
