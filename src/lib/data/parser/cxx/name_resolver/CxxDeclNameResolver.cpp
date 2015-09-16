#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"

#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/logging/logging.h"

CxxDeclNameResolver::CxxDeclNameResolver(const clang::Decl* declaration)
	: CxxNameResolver(std::vector<const clang::Decl*>())
	, m_declaration(declaration)
{
}

CxxDeclNameResolver::CxxDeclNameResolver(const clang::Decl* declaration, std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
	, m_declaration(declaration)
{
}

CxxDeclNameResolver::~CxxDeclNameResolver()
{
}

NameHierarchy CxxDeclNameResolver::getDeclNameHierarchy()
{
	NameHierarchy contextNameHierarchy;
	if (m_declaration)
	{
		std::string declName = "";

		if (clang::isa<clang::NamedDecl>(m_declaration))
		{
			declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(m_declaration));
		}
		else
		{
			LOG_ERROR("unhandled declaration type: " + std::string(m_declaration->getDeclKindName()));
		}

		contextNameHierarchy = getContextNameHierarchy(m_declaration->getDeclContext());

		if ((clang::isa<clang::NonTypeTemplateParmDecl>(m_declaration) ||
			clang::isa<clang::TemplateTypeParmDecl>(m_declaration) ||
			clang::isa<clang::TemplateTemplateParmDecl>(m_declaration)) &&
			contextNameHierarchy.size() > 0)
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

NameHierarchy CxxDeclNameResolver::getContextNameHierarchy(const clang::DeclContext* declContext)
{
	NameHierarchy contextNameHierarchy;

	if (!ignoresContext(declContext))
	{
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
	}
	return contextNameHierarchy;
}

std::string CxxDeclNameResolver::getDeclName()
{
	const clang::NamedDecl* declaration = clang::dyn_cast<clang::NamedDecl>(m_declaration);
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

			clang::TemplateParameterList* parameterList = partialSpecializationDecl->getTemplateParameters();
			unsigned int currentParameterIndex = 0;

			std::string specializedParameterNamePart = "<";
			int templateArgumentCount = partialSpecializationDecl->getTemplateArgs().size();
			const clang::TemplateArgumentList& templateArgumentList = partialSpecializationDecl->getTemplateArgs();
			for (int i = 0; i < templateArgumentCount; i++)
			{
				const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
				if (templateArgument.isDependent()) // IMPORTANT_TODO: fix case when arg depends on template parameter of outer template class, or depends on first template parameter.
				{
					if(currentParameterIndex < parameterList->size())
					{
						specializedParameterNamePart += getTemplateParameterString(parameterList->getParam(currentParameterIndex));
					}
					else
					{
						//this if fixes the crash, but not the problem TODO
						const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
						LOG_ERROR("Template getParam out of Range "+declaration->getLocation().printToString(sourceManager));
					}
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
			std::string templateArgumentNamePart = "<";
			const clang::TemplateArgumentList& templateArgumentList = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration)->getTemplateArgs();
			for (size_t i = 0; i < templateArgumentList.size(); i++)
			{
				templateArgumentNamePart += getTemplateArgumentName(templateArgumentList.get(i));
				templateArgumentNamePart += (i < templateArgumentList.size() - 1) ? ", " : "";
			}
			templateArgumentNamePart += ">";
			declName += templateArgumentNamePart;
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
			std::string templateArgumentNamePart = "<";
			const clang::TemplateArgumentList* templateArgumentList = clang::dyn_cast<clang::FunctionDecl>(declaration)->getTemplateSpecializationArgs();
			for (size_t i = 0; i < templateArgumentList->size(); i++)
			{
				const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
				templateArgumentNamePart += getTemplateArgumentName(templateArgument);
				templateArgumentNamePart += (i < templateArgumentList->size() - 1) ? ", " : "";
			}
			templateArgumentNamePart += ">";
			declName += templateArgumentNamePart;
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

std::string CxxDeclNameResolver::getDeclName(const clang::NamedDecl* declaration)
{
	CxxDeclNameResolver resolver(declaration);
	return resolver.getDeclName();
}

std::string CxxDeclNameResolver::getTemplateParameterString(const clang::NamedDecl* parameter)
{
	std::string templateParameterTypeString = "";

	clang::Decl::Kind templateParameterKind = parameter->getKind();
	switch (templateParameterKind)
	{
	case clang::Decl::NonTypeTemplateParm:
		templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::NonTypeTemplateParmDecl>(parameter));
		break;
	case clang::Decl::TemplateTypeParm:
		templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTypeParmDecl>(parameter));
		break;
	case clang::Decl::TemplateTemplateParm:
		templateParameterTypeString = getTemplateParameterTypeString(clang::dyn_cast<clang::TemplateTemplateParmDecl>(parameter));
		break;
	default:
		LOG_ERROR("Unhandled kind of template parameter.");
	}

	std::string parameterName = parameter->getName();
	if (!parameterName.empty())
	{
		templateParameterTypeString += " " + parameterName;
	}
	return templateParameterTypeString;
}

std::string CxxDeclNameResolver::getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter)
{
	CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());

	if (clang::isa<clang::TemplateDecl>(m_declaration))
	{
		typeNameResolver.ignoreContextDecl(clang::dyn_cast<clang::TemplateDecl>(m_declaration)->getTemplatedDecl());
	}
	else // works for partial template specializations
	{
		typeNameResolver.ignoreContextDecl(m_declaration);
	}
	return typeNameResolver.qualTypeToDataType(parameter->getType())->getFullTypeName();
}

std::string CxxDeclNameResolver::getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter)
{
	return (parameter->wasDeclaredWithTypename() ? "typename" : "class");
}

std::string CxxDeclNameResolver::getTemplateParameterTypeString(const clang::TemplateTemplateParmDecl* parameter)
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

std::string CxxDeclNameResolver::getTemplateArgumentName(const clang::TemplateArgument& argument)
{
	CxxTemplateArgumentNameResolver resolver(getIgnoredContextDecls());
	return resolver.getTemplateArgumentName(argument);
}
