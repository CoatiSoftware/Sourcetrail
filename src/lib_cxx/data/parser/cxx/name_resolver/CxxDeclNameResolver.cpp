#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"

#include <clang/AST/DeclTemplate.h>
#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name/CxxFunctionDeclName.h"
#include "data/parser/cxx/name/CxxVariableDeclName.h"
#include "data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "utility/file/FilePath.h"
#include "utility/ScopedSwitcher.h"

CxxDeclNameResolver::CxxDeclNameResolver()
	: CxxNameResolver(std::vector<const clang::Decl*>())
	, m_currentDecl(nullptr)
{
}

CxxDeclNameResolver::CxxDeclNameResolver(std::vector<const clang::Decl*> ignoredContextDecls)
	: CxxNameResolver(ignoredContextDecls)
	, m_currentDecl(nullptr)
{
}

CxxDeclNameResolver::~CxxDeclNameResolver()
{
}

std::shared_ptr<CxxDeclName> CxxDeclNameResolver::getName(const clang::NamedDecl* declaration)
{
	{
		const clang::Decl* prev = declaration;
		while (prev)
		{
			declaration = clang::dyn_cast_or_null<clang::NamedDecl>(prev);
			prev = prev->getPreviousDecl();
		}
	}

	std::shared_ptr<CxxDeclName> declName;

	if ((declaration) &&
		(clang::isa<clang::CXXRecordDecl>(declaration)) &&
		(clang::dyn_cast<clang::CXXRecordDecl>(declaration)->isLambda()))
	{
		// avoid triggering assert
		clang::DeclarationName Name =
			clang::dyn_cast<clang::CXXRecordDecl>(declaration)->getASTContext().DeclarationNames.getCXXOperatorName(clang::OO_Call);
		clang::DeclContext::lookup_result Calls = clang::dyn_cast<clang::CXXRecordDecl>(declaration)->lookup(Name);
		if (Calls.empty())
		{
			declaration = nullptr;
			declName = std::make_shared<CxxDeclName>("unsolved-lambda", std::vector<std::string>());
		}
		else
		{
			declaration = clang::dyn_cast<clang::CXXRecordDecl>(declaration)->getLambdaCallOperator();
		}
	}

	if (declaration)
	{
		declName = getDeclName(clang::dyn_cast<const clang::NamedDecl>(declaration));

		if (declName)
		{
			if (const clang::UsingDecl* usingDecl = clang::dyn_cast_or_null<clang::UsingDecl>(declaration))
			{
				CxxSpecifierNameResolver specifierNameResolver(getIgnoredContextDecls());
				declName->setParent(specifierNameResolver.getName(usingDecl->getQualifier()));
			}
			else
			{
				declName->setParent(getContextName(declaration->getDeclContext()));
			}
		}
	}
	return declName;
}

std::shared_ptr<CxxName> CxxDeclNameResolver::getContextName(const clang::DeclContext* declContext)
{
	std::shared_ptr<CxxName> contextDeclName;

	if (declContext && !ignoresContext(declContext))
	{
		if (const clang::NamedDecl* contextNamedDecl = clang::dyn_cast_or_null<clang::NamedDecl>(declContext))
		{
			contextDeclName = getDeclName(contextNamedDecl);
			if (contextDeclName)
			{
				contextDeclName->setParent(getContextName(declContext->getParent()));
			}
			else
			{
				contextDeclName = getContextName(declContext->getParent());
			}
		}
	}
	return contextDeclName;
}

std::shared_ptr<CxxDeclName> CxxDeclNameResolver::getDeclName(const clang::NamedDecl* declaration)
{
	if (declaration)
	{
		ScopedSwitcher<const clang::NamedDecl*> switcher(m_currentDecl, declaration);

		std::string declNameString = declaration->getNameAsString();
		if (const clang::TypeAliasDecl* typeAliasDecl = clang::dyn_cast_or_null<clang::TypeAliasDecl>(declaration))
		{
			clang::TypeAliasTemplateDecl* templatedDeclaration = typeAliasDecl->getDescribedAliasTemplate();
			if (templatedDeclaration)
			{
				return getDeclName(templatedDeclaration);
			}
		}
		if (const clang::RecordDecl* recordDecl = clang::dyn_cast_or_null<clang::RecordDecl>(declaration))
		{
			if (recordDecl->isLambda())
			{
				// we skip this node because its child (the lambda call operator) has already been recorded.
				return std::shared_ptr<CxxDeclName>();
			}
			else if (declNameString.size() == 0)
			{
				const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
				const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());

				std::string symbolKindName = "class";
				if (recordDecl->isStruct())
				{
					symbolKindName = "struct";
				}
				else if (recordDecl->isUnion())
				{
					symbolKindName = "union";
				}

				return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol(symbolKindName, presumedBegin), std::vector<std::string>());
			}
			else if (const clang::CXXRecordDecl* cxxRecordDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(declaration))
			{
				clang::ClassTemplateDecl* templateClassDeclaration = cxxRecordDecl->getDescribedClassTemplate();
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

					std::vector<std::string> templateParameters;
					const clang::TemplateArgumentList& templateArgumentList = partialSpecializationDecl->getTemplateArgs();
					const int templateArgumentCount = templateArgumentList.size();
					for (int i = 0; i < templateArgumentCount; i++)
					{
						const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
						if (templateArgument.isDependent()) //  IMPORTANT_TODO: fix case when arg depends on template parameter of outer template class, or depends on first template parameter.
						{
							if(currentParameterIndex < parameterList->size())
							{
								templateParameters.push_back(getTemplateParameterString(parameterList->getParam(currentParameterIndex)));
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
							templateParameters.push_back(getTemplateArgumentName(templateArgument));
						}
					}

					return std::make_shared<CxxDeclName>(declNameString, templateParameters);
				}
				else if (clang::isa<clang::ClassTemplateSpecializationDecl>(declaration))
				{
					std::vector<std::string> templateArguments;
					const clang::TemplateArgumentList& templateArgumentList = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(declaration)->getTemplateArgs();
					for (size_t i = 0; i < templateArgumentList.size(); i++)
					{
						templateArguments.push_back(getTemplateArgumentName(templateArgumentList.get(i)));
					}
					return std::make_shared<CxxDeclName>(declNameString, templateArguments);
				}
			}
		}
		else if (clang::isa<clang::FunctionDecl>(declaration))
		{
			const clang::FunctionDecl* functionDecl = clang::dyn_cast<clang::FunctionDecl>(declaration);

			std::string functionName = declNameString;
			std::vector<std::string> templateArguments;

			if ((clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)) &&
				(clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)->getParent()->isLambda()))
			{
				const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
				const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)->getParent()->getLocStart());
				functionName = "lambda at " + std::to_string(presumedBegin.getLine()) + ":" + std::to_string(presumedBegin.getColumn());
			}
			else if (clang::FunctionTemplateDecl* templateFunctionDeclaration = functionDecl->getDescribedFunctionTemplate())
			{
				std::shared_ptr<CxxDeclName> templateDeclName = getDeclName(templateFunctionDeclaration);
				functionName = templateDeclName->getName();
				templateArguments = templateDeclName->getTemplateParameterNames();
			}
			else
			{
				if (functionDecl->isFunctionTemplateSpecialization())
				{
					const clang::TemplateArgumentList* templateArgumentList = functionDecl->getTemplateSpecializationArgs();
					for (size_t i = 0; i < templateArgumentList->size(); i++)
					{
						const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
						templateArguments.push_back(getTemplateArgumentName(templateArgument));
					}
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
			std::shared_ptr<CxxTypeName> returnTypeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(functionDecl->getReturnType()));

			std::vector<std::shared_ptr<CxxTypeName>> parameterTypeNames;
			for (unsigned int i = 0; i < functionDecl->param_size(); i++)
			{
				parameterTypeNames.push_back(CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(functionDecl->parameters()[i]->getType())));
			}

			return std::make_shared<CxxFunctionDeclName>(
				functionName,
				templateArguments,
				returnTypeName,
				parameterTypeNames,
				isConst,
				isStatic
			);
		}
		else if (clang::isa<clang::TemplateDecl>(declaration)) // also triggers on TemplateTemplateParmDecl
		{
			std::vector<std::string> templateParameters;
			clang::TemplateParameterList* parameterList = clang::dyn_cast<clang::TemplateDecl>(declaration)->getTemplateParameters();
			for (size_t i = 0; i < parameterList->size(); i++)
			{
				templateParameters.push_back(getTemplateParameterString(parameterList->getParam(i)));
			}
			return std::make_shared<CxxDeclName>(declNameString, templateParameters);
		}
		else if (clang::isa<clang::FieldDecl>(declaration))
		{
			const clang::FieldDecl* fieldDecl = clang::dyn_cast<clang::FieldDecl>(declaration);
			CxxTypeNameResolver typenNameResolver(getIgnoredContextDecls());
			typenNameResolver.ignoreContextDecl(fieldDecl);
			std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(fieldDecl->getType()));
			return std::make_shared<CxxVariableDeclName>(declNameString, std::vector<std::string>(), typeName, false);
		}
		else if (clang::isa<clang::NamespaceDecl>(declaration) && clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("namespace", presumedBegin), std::vector<std::string>());
		}
		else if (clang::isa<clang::EnumDecl>(declaration) && declNameString.size() == 0)
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("enum", presumedBegin), std::vector<std::string>());
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
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("template parameter", presumedBegin), std::vector<std::string>());
		}
		else if (clang::isa<clang::ParmVarDecl>(declaration) && declNameString.size() == 0)
		{
			const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
			const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("parameter", presumedBegin), std::vector<std::string>());
		}
		else if (clang::isa<clang::VarDecl>(declaration))
		{
			const clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declaration);
			if (varDecl->getParentFunctionOrMethod() == NULL)
			{
				bool isStatic = false;
				if (varDecl->getAccess() != clang::AS_none)
				{
					// var is declared inside a type and must be static (non-statics are stored as clang::FieldDecl)
					isStatic = true;
				}
				else
				{
					// nothing todo, varDecl is global (and non-static)
				}

				CxxTypeNameResolver typenNameResolver(getIgnoredContextDecls());
				typenNameResolver.ignoreContextDecl(varDecl);
				std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(varDecl->getType()));
				return std::make_shared<CxxVariableDeclName>(declNameString, std::vector<std::string>(), typeName, isStatic);
			}
		}

		if (declNameString.size() > 0)
		{
			return std::make_shared<CxxDeclName>(declNameString, std::vector<std::string>(), std::shared_ptr<CxxName>());
		}
	}

	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());
	// LOG_ERROR("could not resolve name of decl at: " + declaration->getLocation().printToString(sourceManager));
	return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("symbol", presumedBegin), std::vector<std::string>());
}

std::string CxxDeclNameResolver::getNameForAnonymousSymbol(const std::string& symbolKindName, const clang::PresumedLoc& presumedBegin)
{
	if (presumedBegin.isValid())
	{
		return "anonymous " + symbolKindName +
			" (" + FilePath(presumedBegin.getFilename()).fileName() + "<" + std::to_string(presumedBegin.getLine()) + ":" + std::to_string(presumedBegin.getColumn()) + ">)";
	}
	return "anonymous " + symbolKindName;
}

std::string CxxDeclNameResolver::getTemplateParameterString(const clang::NamedDecl* parameter)
{
	std::string templateParameterTypeString = "";

	if (parameter)
	{
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
	}
	return templateParameterTypeString;
}

std::string CxxDeclNameResolver::getTemplateParameterTypeString(const clang::NonTypeTemplateParmDecl* parameter)
{
	CxxTypeNameResolver typeNameResolver(getIgnoredContextDecls());

	if (clang::isa<clang::TemplateDecl>(m_currentDecl))
	{
		typeNameResolver.ignoreContextDecl(clang::dyn_cast<clang::TemplateDecl>(m_currentDecl)->getTemplatedDecl());
	}
	else // works for partial template specializations
	{
		typeNameResolver.ignoreContextDecl(m_currentDecl);
	}

	std::string typeString = "";

	std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typeNameResolver.getName(parameter->getType()));
	typeString = typeName->toString();

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
