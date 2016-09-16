#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"

#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"

CxxDeclNameResolver::CxxDeclNameResolver(const clang::Decl* declaration)
	: CxxNameResolver(std::vector<const clang::Decl*>())
{
	const clang::Decl* prev = declaration;
	while (prev)
	{
		m_declaration = prev;
		prev = prev->getPreviousDecl();
	}
}

CxxDeclNameResolver::CxxDeclNameResolver(const clang::Decl* declaration, std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
{
	const clang::Decl* prev = declaration;
	while (prev)
	{
		m_declaration = prev;
		prev = prev->getPreviousDecl();
	}
}

CxxDeclNameResolver::~CxxDeclNameResolver()
{
}

NameHierarchy CxxDeclNameResolver::getDeclNameHierarchy()
{
	NameHierarchy contextNameHierarchy;
	if (m_declaration)
	{
		std::shared_ptr<NameElement> declName;

		if (clang::isa<clang::NamedDecl>(m_declaration))
		{
			declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(m_declaration));
		}
		else
		{
			// LOG_ERROR("unhandled declaration type: " + std::string(m_declaration->getDeclKindName()));
		}

		contextNameHierarchy = getContextNameHierarchy(m_declaration->getDeclContext());

		if (declName)
		{
			contextNameHierarchy.push(declName);
		}
	}
	return contextNameHierarchy;
}

NameHierarchy CxxDeclNameResolver::getContextNameHierarchy(const clang::DeclContext* declContext)
{
	NameHierarchy contextNameHierarchy;

	if (declContext && !ignoresContext(declContext))
	{
		const clang::DeclContext* parentContext = declContext->getParent();
		if (parentContext)
		{
			contextNameHierarchy = getContextNameHierarchy(parentContext);
		}

		if (const clang::NamedDecl* contextNamedDecl = clang::dyn_cast_or_null<clang::NamedDecl>(declContext))
		{
			std::shared_ptr<NameElement> declName = getDeclName(contextNamedDecl);
			if (declName)
			{
				contextNameHierarchy.push(declName);
			}
		}
	}
	return contextNameHierarchy;
}

std::shared_ptr<NameElement> CxxDeclNameResolver::getDeclName()
{
	const clang::NamedDecl* declaration = clang::dyn_cast<clang::NamedDecl>(m_declaration);
	std::string declNameString = declaration->getNameAsString();
	if (const clang::TypeAliasDecl* typeAliasDecl = clang::dyn_cast_or_null<clang::TypeAliasDecl>(declaration))
	{
		clang::TypeAliasTemplateDecl* templatedDeclaration = typeAliasDecl->getDescribedAliasTemplate();
		if (templatedDeclaration)
		{
			return getDeclName(templatedDeclaration);
		}
	}
	if (const clang::CXXRecordDecl* recordDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(declaration))
	{
		clang::ClassTemplateDecl* templateClassDeclaration = recordDecl->getDescribedClassTemplate();
		if (templateClassDeclaration)
		{
			return getDeclName(templateClassDeclaration);
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
				if (templateArgument.isDependent()) //  IMPORTANT_TODO: fix case when arg depends on template parameter of outer template class, or depends on first template parameter.
				{
					if(currentParameterIndex < parameterList->size())
					{
						specializedParameterNamePart += getTemplateParameterString(parameterList->getParam(currentParameterIndex));
					}
					else
					{
						//this if fixes the crash, but not the problem TODO
						// const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
						// LOG_ERROR("Template getParam out of Range " + declaration->getLocation().printToString(sourceManager));
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
			return std::make_shared<NameElement>(declNameString + specializedParameterNamePart);
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
			return std::make_shared<NameElement>(declNameString + templateArgumentNamePart);
		}
		else if (recordDecl->isLambda())
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(recordDecl->getLocStart());
			std::string lambdaName = "lambda at " + std::to_string(presumedBegin.getLine()) + ":" + std::to_string(presumedBegin.getColumn());
			return std::make_shared<NameElement>(lambdaName, NameElement::Signature("void", "()"));
		}
		else if (declNameString.size() == 0)
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
			const std::string recordType = (recordDecl->isStruct() ? "struct" : "class");
			return std::make_shared<NameElement>("anonymous " + recordType + " (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
		}
	}
	else if (clang::isa<clang::FunctionDecl>(declaration))
	{
		if (const clang::CXXMethodDecl* methodDecl = clang::dyn_cast_or_null<clang::CXXMethodDecl>(declaration))
		{
			if (methodDecl->getParent()->isLambda())
			{
				// return empty pointer since lambdas will be handled at the level of the parent class... not optimal.
				return std::shared_ptr<NameElement>();
			}
		}

		std::string functionName;

		const clang::FunctionDecl* functionDecl = clang::dyn_cast<clang::FunctionDecl>(declaration);
		if (clang::FunctionTemplateDecl* templateFunctionDeclaration = functionDecl->getDescribedFunctionTemplate())
		{
			functionName = getDeclName(templateFunctionDeclaration)->getName();
		}
		else
		{
			functionName = declNameString;
			if (functionDecl->isFunctionTemplateSpecialization())
			{
				std::string templateArgumentNamePart = "<";
				const clang::TemplateArgumentList* templateArgumentList = functionDecl->getTemplateSpecializationArgs();
				for (size_t i = 0; i < templateArgumentList->size(); i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
					templateArgumentNamePart += getTemplateArgumentName(templateArgument);
					templateArgumentNamePart += (i < templateArgumentList->size() - 1) ? ", " : "";
				}
				templateArgumentNamePart += ">";
				functionName += templateArgumentNamePart;
			}
		}

		bool isStatic = false;
		bool isConst = false;

		if (clang::isa<clang::CXXMethodDecl>(declaration))
		{
			const clang::CXXMethodDecl* methodDecl = clang::dyn_cast<const clang::CXXMethodDecl>(declaration);
			isStatic = methodDecl->isStatic();
			isConst = methodDecl->isConst();
		}
		else
		{
			isStatic = functionDecl->getStorageClass() == clang::SC_Static;
		}

		CxxTypeNameResolver typenNameResolver(getIgnoredContextDecls());
		typenNameResolver.ignoreContextDecl(functionDecl);
		std::string returnTypeString = typenNameResolver.qualTypeToDataType(functionDecl->getReturnType())->getFullTypeName();

		std::string parameterString = "(";
		for (unsigned int i = 0; i < functionDecl->param_size(); i++)
		{
			if (i > 0)
			{
				parameterString += ", ";
			}
			parameterString += typenNameResolver.qualTypeToDataType(functionDecl->parameters()[i]->getType())->getFullTypeName();
		}
		parameterString += ")";

		return std::make_shared<NameElement>(
			functionName,
			NameElement::Signature((isStatic ? "static " : "") + returnTypeString, parameterString + (isConst ? " const" : "")));
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
		return std::make_shared<NameElement>(declNameString + templateParameterNamePart);
	}
	else if (clang::isa<clang::NamespaceDecl>(declaration) && clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
	{
		const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
		return std::make_shared<NameElement>("anonymous namespace (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
	}
	else if (clang::isa<clang::EnumDecl>(declaration) && declNameString.size() == 0)
	{
		const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
		return std::make_shared<NameElement>("anonymous enum (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
	}
	else if (
		(
			clang::isa<clang::TemplateTypeParmDecl>(declaration) ||
			clang::isa<clang::NonTypeTemplateParmDecl>(declaration) ||
			clang::isa<clang::TemplateTemplateParmDecl>(declaration)
		) && declNameString.size() == 0)
	{
		const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
		return std::make_shared<NameElement>("anonymous template parameter (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
	}
	else if (clang::isa<clang::ParmVarDecl>(declaration) && declNameString.size() == 0)
	{
		const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
		return std::make_shared<NameElement>("anonymous parameter (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
	}

	if (declNameString.size() > 0)
	{
		return std::make_shared<NameElement>(declNameString);
	}

	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
	// LOG_ERROR("could not resolve name of decl at: " + declaration->getLocation().printToString(sourceManager));
	return std::make_shared<NameElement>("anonymous symbol (" + FilePath(presumedBegin.getFilename()).fileName() + ")");
}

std::shared_ptr<NameElement> CxxDeclNameResolver::getDeclName(const clang::NamedDecl* declaration)
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
		// LOG_ERROR("Unhandled kind of template parameter.");
		break;
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

	std::string typeString = typeNameResolver.qualTypeToDataType(parameter->getType())->getFullTypeName();

	if (parameter->isTemplateParameterPack())
	{
		typeString += "...";
	}

	return typeString;
}

std::string CxxDeclNameResolver::getTemplateParameterTypeString(const clang::TemplateTypeParmDecl* parameter)
{
	std::string typeString = (parameter->wasDeclaredWithTypename() ? "typename" : "class");
	if (parameter->isTemplateParameterPack())
	{
		typeString += "...";
	}
	return typeString;
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

	if (parameter->isTemplateParameterPack())
	{
		templateParameterTypeString += "...";
	}

	return templateParameterTypeString;
}

std::string CxxDeclNameResolver::getTemplateArgumentName(const clang::TemplateArgument& argument)
{
	CxxTemplateArgumentNameResolver resolver(getIgnoredContextDecls());
	return resolver.getTemplateArgumentName(argument);
}
