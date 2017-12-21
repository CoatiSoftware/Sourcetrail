#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"

#include <clang/AST/ASTContext.h>

#include "data/parser/cxx/name/CxxFunctionDeclName.h"
#include "data/parser/cxx/name/CxxVariableDeclName.h"
#include "data/parser/cxx/name/CxxStaticFunctionDeclName.h"
#include "data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/utilityClang.h"
#include "utility/file/FilePath.h"
#include "utility/ScopedSwitcher.h"
#include "utility/utilityString.h"

CxxDeclNameResolver::CxxDeclNameResolver(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache, std::vector<const clang::Decl*>())
	, m_currentDecl(nullptr)
{
}

CxxDeclNameResolver::CxxDeclNameResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: CxxNameResolver(canonicalFilePathCache, ignoredContextDecls)
	, m_currentDecl(nullptr)
{
}

CxxDeclNameResolver::~CxxDeclNameResolver()
{
}

std::shared_ptr<CxxDeclName> CxxDeclNameResolver::getName(const clang::NamedDecl* declaration)
{
	declaration = utility::getFirstDecl(declaration);

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
				CxxSpecifierNameResolver specifierNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				declName->setParent(specifierNameResolver.getName(usingDecl->getQualifier()));
			}
			else if (
				clang::isa<clang::TemplateTypeParmDecl>(declaration) ||
				clang::isa<clang::NonTypeTemplateParmDecl>(declaration) ||
				clang::isa<clang::TemplateTemplateParmDecl>(declaration)
			) {
				clang::ASTContext& context = declaration->getASTContext();

				clang::ASTContext::DynTypedNodeList parents = context.getParents(*declaration);
				for (const clang::ast_type_traits::DynTypedNode* parent = parents.begin(); parent != parents.end(); parent++)
				{
					const clang::Decl* parentDecl = parent->get<clang::Decl>();
					while (parentDecl != nullptr)
					{
						parentDecl = utility::getFirstDecl(parentDecl);

						if (clang::isa<clang::TemplateDecl>(parentDecl))
						{
							const clang::TemplateDecl* parentTemplateDecl = clang::dyn_cast_or_null<clang::TemplateDecl>(parentDecl);
							if (!ignoresContext(parentTemplateDecl) && !ignoresContext(parentTemplateDecl->getTemplatedDecl()))
							{
								declName->setParent(getName(parentTemplateDecl));
							}
							break;
						}
						else if (clang::isa<clang::ClassTemplatePartialSpecializationDecl>(parentDecl))
						{
							const clang::ClassTemplatePartialSpecializationDecl* parentClassTemplateDecl = clang::dyn_cast_or_null<clang::ClassTemplatePartialSpecializationDecl>(parentDecl);
							if (!ignoresContext(parentDecl))
							{
								declName->setParent(getName(parentClassTemplateDecl));
							}
							break;
						}
						else if (clang::isa<clang::VarTemplatePartialSpecializationDecl>(parentDecl))
						{
							const clang::VarTemplatePartialSpecializationDecl* parentVarTemplateDecl = clang::dyn_cast_or_null<clang::VarTemplatePartialSpecializationDecl>(parentDecl);
							if (!ignoresContext(parentDecl))
							{
								declName->setParent(getName(parentVarTemplateDecl));
							}
							break;
						}

						if (const clang::DeclContext* parentDeclContext = parentDecl->getDeclContext())
						{
							if (ignoresContext(parentDeclContext))
							{
								break;
							}
							parentDecl = clang::dyn_cast_or_null<clang::Decl>(parentDeclContext);
							if (parentDecl)
							{
								if (clang::TemplateDecl* describedTemplate = parentDecl->getDescribedTemplate())
								{
									parentDecl = describedTemplate;
								}
							}
						}
						else
						{
							break;
						}
					}
				}
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
		if (const clang::TagDecl* tagDecl = clang::dyn_cast_or_null<clang::TagDecl>(declaration))
		{
			if (const clang::TypedefNameDecl* typedefNameDecl = tagDecl->getTypedefNameForAnonDecl())
			{
				declNameString = typedefNameDecl->getNameAsString();
			}
		}

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
			else if (declNameString.empty())
			{
				std::string symbolKindName = "class";
				if (recordDecl->isStruct())
				{
					symbolKindName = "struct";
				}
				else if (recordDecl->isUnion())
				{
					symbolKindName = "union";
				}

				return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol(symbolKindName, declaration), std::vector<std::string>());
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
					const std::vector<std::string> templateParameterNames = getTemplateParameterStringsOfPatrialSpecialitarion(
						clang::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(declaration)
					);
					return std::make_shared<CxxDeclName>(declNameString, templateParameterNames);
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
				templateArguments = getTemplateParameterStrings(templateFunctionDeclaration);
			}
			else if (functionDecl->isFunctionTemplateSpecialization())
			{
				const clang::TemplateArgumentList* templateArgumentList = functionDecl->getTemplateSpecializationArgs();
				for (size_t i = 0; i < templateArgumentList->size(); i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
					templateArguments.push_back(getTemplateArgumentName(templateArgument));
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

			CxxTypeNameResolver typenNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
			typenNameResolver.ignoreContextDecl(functionDecl);
			std::shared_ptr<CxxTypeName> returnTypeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(functionDecl->getReturnType()));

			std::vector<std::shared_ptr<CxxTypeName>> parameterTypeNames;
			for (unsigned int i = 0; i < functionDecl->param_size(); i++)
			{
				parameterTypeNames.push_back(CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(functionDecl->parameters()[i]->getType())));
			}

			if (!clang::isa<clang::CXXMethodDecl>(declaration) && isStatic)
			{
				return std::make_shared<CxxStaticFunctionDeclName>(
					functionName,
					templateArguments,
					returnTypeName,
					parameterTypeNames,
					getTranslationUnitMainFileName(declaration)
				);
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
		else if (clang::isa<clang::FunctionTemplateDecl>(declaration))
		{
			const clang::FunctionTemplateDecl* functionTemplateDecl = clang::dyn_cast<clang::FunctionTemplateDecl>(declaration);
			return getDeclName(functionTemplateDecl->getTemplatedDecl());
		}
		else if (clang::isa<clang::FieldDecl>(declaration))
		{
			const clang::FieldDecl* fieldDecl = clang::dyn_cast<clang::FieldDecl>(declaration);
			CxxTypeNameResolver typenNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
			typenNameResolver.ignoreContextDecl(fieldDecl);
			std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(fieldDecl->getType()));
			return std::make_shared<CxxVariableDeclName>(declNameString, std::vector<std::string>(), typeName, false);
		}
		else if (clang::isa<clang::NamespaceDecl>(declaration) && clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
		{
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("namespace", declaration), std::vector<std::string>());
		}
		else if (clang::isa<clang::EnumDecl>(declaration) && declNameString.empty())
		{
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("enum", declaration), std::vector<std::string>());
		}
		else if (
			(
				clang::isa<clang::TemplateTypeParmDecl>(declaration) ||
				clang::isa<clang::NonTypeTemplateParmDecl>(declaration) ||
				clang::isa<clang::TemplateTemplateParmDecl>(declaration)
			) && declNameString.empty())
		{
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("template parameter", declaration), std::vector<std::string>());
		}
		else if (clang::isa<clang::ParmVarDecl>(declaration) && declNameString.empty())
		{
			return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("parameter", declaration), std::vector<std::string>());
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

				CxxTypeNameResolver typenNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
				typenNameResolver.ignoreContextDecl(varDecl);
				std::shared_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(typenNameResolver.getName(varDecl->getType()));

				std::string varName = declNameString;
				if (utility::getSymbolKind(varDecl) == SYMBOL_GLOBAL_VARIABLE &&
					varDecl->getStorageClass() == clang::SC_Static)
				{
					// if a global variable is static it is only visible in the current translation unit. Therefore if multiple instances of that global variable
					// may be generated (one for each translation unit) we add the name of the translation unit's source file.
					// If that global variable definition is const, we add the name of the (maybe header) file that variable is defined in instead. This causes
					// different instances of the variable that all MUST contain the same value to be merged into a single node in Sourcetrail.
					std::string scopeFileName = "";
					{
						if (varDecl->getType().isConstQualified())
						{
							scopeFileName = getDeclarationFileName(declaration);
						}
						else
						{
							scopeFileName = getTranslationUnitMainFileName(declaration);
						}
					}
					if (!scopeFileName.empty())
					{
						varName = declNameString + " (" + scopeFileName + ")";
					}
				}

				std::vector<std::string> templateParameterNames;
				if (varDecl->getDescribedVarTemplate())
				{
					const clang::VarTemplateDecl* templateDeclaration = varDecl->getDescribedVarTemplate();
					templateParameterNames = getTemplateParameterStrings(templateDeclaration);
				}
				else if (clang::isa<clang::VarTemplatePartialSpecializationDecl>(declaration))
				{
					templateParameterNames = getTemplateParameterStringsOfPatrialSpecialitarion(clang::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration));
				}
				else if (clang::isa<clang::VarTemplateSpecializationDecl>(declaration))
				{
					const clang::VarTemplateSpecializationDecl* templateSpecializationDeclaration = clang::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(varDecl);
					const clang::TemplateArgumentList& templateArgumentList = templateSpecializationDeclaration->getTemplateArgs();
					for (size_t i = 0; i < templateArgumentList.size(); i++)
					{
						const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
						templateParameterNames.push_back(getTemplateArgumentName(templateArgument));
					}
				}

				return std::make_shared<CxxVariableDeclName>(varName, templateParameterNames, typeName, isStatic);
			}
		}
		else if (clang::isa<clang::VarTemplateDecl>(declaration))
		{
			const clang::VarTemplateDecl* varTemplateDecl = clang::dyn_cast<clang::VarTemplateDecl>(declaration);
			return getDeclName(varTemplateDecl->getTemplatedDecl());
		}
		else if (clang::isa<clang::TemplateDecl>(declaration)) // also triggers on TemplateTemplateParmDecl
		{
			return std::make_shared<CxxDeclName>(declNameString, getTemplateParameterStrings(clang::dyn_cast<clang::TemplateDecl>(declaration)));
		}

		if (!declNameString.empty())
		{
			return std::make_shared<CxxDeclName>(declNameString, std::vector<std::string>(), std::shared_ptr<CxxName>());
		}
	}

	// LOG_ERROR("could not resolve name of decl at: " + declaration->getLocation().printToString(sourceManager));
	return std::make_shared<CxxDeclName>(getNameForAnonymousSymbol("symbol", declaration), std::vector<std::string>());
}

std::string CxxDeclNameResolver::getTranslationUnitMainFileName(const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	clang::FileID fileId = sourceManager.getMainFileID();
	if (fileId.isValid())
	{
		const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
		return getCanonicalFilePathCache()->getCanonicalFilePath(fileEntry).fileName();
	}
	return "";
}

std::string CxxDeclNameResolver::getDeclarationFileName(const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(sourceManager.getFileID(declaration->getLocStart()));
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		return getCanonicalFilePathCache()->getCanonicalFilePath(fileEntry).fileName();
	}
	return getCanonicalFilePathCache()->getCanonicalFilePath(sourceManager.getPresumedLoc(declaration->getLocStart()).getFilename()).fileName();
}

std::string CxxDeclNameResolver::getNameForAnonymousSymbol(const std::string& symbolKindName, const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getLocStart());

	if (presumedBegin.isValid())
	{
		return "anonymous " + symbolKindName +
			" (" + getDeclarationFileName(declaration) + "<" + std::to_string(presumedBegin.getLine()) + ":" + std::to_string(presumedBegin.getColumn()) + ">)";
	}
	return "anonymous " + symbolKindName;
}

std::vector<std::string> CxxDeclNameResolver::getTemplateParameterStrings(const clang::TemplateDecl* templateDecl)
{
	std::vector<std::string> templateParameterStrings;
	clang::TemplateParameterList* parameterList = templateDecl->getTemplateParameters();
	for (size_t i = 0; i < parameterList->size(); i++)
	{
		templateParameterStrings.push_back(getTemplateParameterString(parameterList->getParam(i)));
	}
	return templateParameterStrings;
}

std::string CxxDeclNameResolver::getTemplateParameterString(const clang::NamedDecl* parameter)
{
	std::string templateParameterTypeString;

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
	CxxTypeNameResolver typeNameResolver(getCanonicalFilePathCache(), getIgnoredContextDecls());

	if (clang::isa<clang::TemplateDecl>(m_currentDecl))
	{
		typeNameResolver.ignoreContextDecl(clang::dyn_cast<clang::TemplateDecl>(m_currentDecl)->getTemplatedDecl());
	}
	else // works for partial template specializations
	{
		typeNameResolver.ignoreContextDecl(m_currentDecl);
	}

	std::string typeString;

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
	CxxTemplateArgumentNameResolver resolver(getCanonicalFilePathCache(), getIgnoredContextDecls());
	return resolver.getTemplateArgumentName(argument);
}
