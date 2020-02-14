#include "CxxDeclNameResolver.h"

#include <clang/AST/ASTContext.h>

#include "CanonicalFilePathCache.h"
#include "CxxFunctionDeclName.h"
#include "CxxSpecifierNameResolver.h"
#include "CxxStaticFunctionDeclName.h"
#include "CxxTemplateArgumentNameResolver.h"
#include "CxxTemplateParameterStringResolver.h"
#include "CxxTypeNameResolver.h"
#include "CxxVariableDeclName.h"
#include "ScopedSwitcher.h"
#include "utilityClang.h"
#include "utilityString.h"

CxxDeclNameResolver::CxxDeclNameResolver(CanonicalFilePathCache* canonicalFilePathCache)
	: CxxNameResolver(canonicalFilePathCache), m_currentDecl(nullptr)
{
}

CxxDeclNameResolver::CxxDeclNameResolver(const CxxNameResolver* other)
	: CxxNameResolver(other), m_currentDecl(nullptr)
{
}

std::unique_ptr<CxxDeclName> CxxDeclNameResolver::getName(const clang::NamedDecl* declaration)
{
	declaration = utility::getFirstDecl(declaration);

	if ((declaration) && (clang::isa<clang::CXXRecordDecl>(declaration)) &&
		(clang::dyn_cast<clang::CXXRecordDecl>(declaration)->isLambda()))
	{
		// avoid triggering assert
		clang::DeclarationName Name = clang::dyn_cast<clang::CXXRecordDecl>(declaration)
										  ->getASTContext()
										  .DeclarationNames.getCXXOperatorName(clang::OO_Call);
		clang::DeclContext::lookup_result Calls =
			clang::dyn_cast<clang::CXXRecordDecl>(declaration)->lookup(Name);
		if (Calls.empty())
		{
			return std::make_unique<CxxDeclName>(L"unsolved-lambda");
		}
		else
		{
			declaration = clang::dyn_cast<clang::CXXRecordDecl>(declaration)->getLambdaCallOperator();
		}
	}

	if (!declaration)
	{
		return nullptr;
	}

	std::unique_ptr<CxxDeclName> declName = getDeclName(
		clang::dyn_cast<const clang::NamedDecl>(declaration));
	if (declName)
	{
		if (const clang::UsingDecl* usingDecl = clang::dyn_cast_or_null<clang::UsingDecl>(declaration))
		{
			declName->setParent(CxxSpecifierNameResolver(this).getName(usingDecl->getQualifier()));
		}
		else if (
			!clang::isa<clang::TemplateTypeParmDecl>(declaration) &&
			!clang::isa<clang::NonTypeTemplateParmDecl>(declaration) &&
			!clang::isa<clang::TemplateTemplateParmDecl>(declaration))
		{
			declName->setParent(getContextName(declaration->getDeclContext()));
		}
	}

	return declName;
}

std::unique_ptr<CxxName> CxxDeclNameResolver::getContextName(const clang::DeclContext* declContext)
{
	std::unique_ptr<CxxName> contextDeclName;

	if (declContext && !ignoresContext(declContext))
	{
		if (const clang::NamedDecl* contextNamedDecl = clang::dyn_cast_or_null<clang::NamedDecl>(
				declContext))
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

std::unique_ptr<CxxDeclName> CxxDeclNameResolver::getDeclName(const clang::NamedDecl* declaration)
{
	if (declaration)
	{
		ScopedSwitcher<const clang::NamedDecl*> switcher(m_currentDecl, declaration);

		std::wstring declNameString = utility::decodeFromUtf8(declaration->getNameAsString());

		if (const clang::TagDecl* tagDecl = clang::dyn_cast_or_null<clang::TagDecl>(declaration))
		{
			if (const clang::TypedefNameDecl* typedefNameDecl = tagDecl->getTypedefNameForAnonDecl())
			{
				declNameString = utility::decodeFromUtf8(typedefNameDecl->getNameAsString());
			}
		}

		if (const clang::TypeAliasDecl* typeAliasDecl =
				clang::dyn_cast_or_null<clang::TypeAliasDecl>(declaration))
		{
			clang::TypeAliasTemplateDecl* templatedDeclaration =
				typeAliasDecl->getDescribedAliasTemplate();
			if (templatedDeclaration)
			{
				return getDeclName(templatedDeclaration);
			}
		}

		if (const clang::RecordDecl* recordDecl = clang::dyn_cast_or_null<clang::RecordDecl>(
				declaration))
		{
			if (recordDecl->isLambda())
			{
				// we skip this node because its child (the lambda call operator) has already been recorded.
				return std::unique_ptr<CxxDeclName>();
			}
			else if (declNameString.empty())
			{
				std::wstring symbolKindName = L"class";
				if (recordDecl->isStruct())
				{
					symbolKindName = L"struct";
				}
				else if (recordDecl->isUnion())
				{
					symbolKindName = L"union";
				}

				return std::make_unique<CxxDeclName>(
					getNameForAnonymousSymbol(symbolKindName, declaration));
			}
			else if (const clang::CXXRecordDecl* cxxRecordDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(declaration))
			{
				if (clang::ClassTemplateDecl* templateClassDeclaration =
						cxxRecordDecl->getDescribedClassTemplate())
				{
					return getDeclName(templateClassDeclaration);
				}
				else if (clang::isa<clang::ClassTemplatePartialSpecializationDecl>(declaration))
				{
					return std::make_unique<CxxDeclName>(
						std::move(declNameString),
						getTemplateParameterStringsOfPartialSpecialization(
							clang::dyn_cast<clang::ClassTemplatePartialSpecializationDecl>(
								declaration)));
				}
				else if (
					const clang::ClassTemplateSpecializationDecl* templateSpecialitarionDecl =
						clang::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(declaration))
				{
					std::vector<std::wstring> templateArguments;
					const clang::TemplateArgumentList& templateArgumentList =
						templateSpecialitarionDecl->getTemplateArgs();
					for (unsigned i = 0; i < templateArgumentList.size(); i++)
					{
						if (templateArgumentList.get(i).isDependent())
						{
							llvm::PointerUnion<clang::ClassTemplateDecl*, clang::ClassTemplatePartialSpecializationDecl*>
								pu = templateSpecialitarionDecl->getSpecializedTemplateOrPartial();
							if (pu.is<clang::ClassTemplateDecl*>())
							{
								return getDeclName(pu.get<clang::ClassTemplateDecl*>());
							}
							else if (pu.is<clang::ClassTemplatePartialSpecializationDecl*>())
							{
								return getDeclName(
									pu.get<clang::ClassTemplatePartialSpecializationDecl*>());
							}
						}

						templateArguments.push_back(
							getTemplateArgumentName(templateArgumentList.get(i)));
					}
					return std::make_unique<CxxDeclName>(
						std::move(declNameString), std::move(templateArguments));
				}
			}
		}
		else if (clang::isa<clang::FunctionDecl>(declaration))
		{
			const clang::FunctionDecl* functionDecl = clang::dyn_cast<clang::FunctionDecl>(
				declaration);

			std::wstring functionName = declNameString;
			std::vector<std::wstring> templateArguments;

			if ((clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)) &&
				(clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)->getParent()->isLambda()))
			{
				const clang::SourceManager& sourceManager =
					declaration->getASTContext().getSourceManager();
				const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(
					clang::dyn_cast_or_null<clang::CXXMethodDecl>(functionDecl)
						->getParent()
						->getBeginLoc());
				functionName = L"lambda at " + std::to_wstring(presumedBegin.getLine()) + L":" +
					std::to_wstring(presumedBegin.getColumn());
			}
			else if (clang::FunctionTemplateDecl* templateFunctionDeclaration = functionDecl->getDescribedFunctionTemplate())
			{
				templateArguments = getTemplateParameterStrings(templateFunctionDeclaration);
			}
			else if (functionDecl->isFunctionTemplateSpecialization())
			{
				const clang::TemplateArgumentList* templateArgumentList =
					functionDecl->getTemplateSpecializationArgs();
				for (unsigned i = 0; i < templateArgumentList->size(); i++)
				{
					const clang::TemplateArgument& templateArgument = templateArgumentList->get(i);
					if (templateArgument.isDependent())
					{
						if (clang::FunctionTemplateDecl* templateFunctionDeclaration =
								functionDecl->getPrimaryTemplate())
						{
							return getDeclName(templateFunctionDeclaration);
						}
					}
					templateArguments.push_back(getTemplateArgumentName(templateArgument));
				}
			}

			bool isStatic = false;
			bool isConst = false;

			if (clang::isa<clang::CXXMethodDecl>(declaration))
			{
				const clang::CXXMethodDecl* methodDecl = clang::dyn_cast<const clang::CXXMethodDecl>(
					declaration);
				isStatic = methodDecl->isStatic();
				isConst = methodDecl->isConst();
			}
			else
			{
				isStatic = functionDecl->getStorageClass() == clang::SC_Static;
			}

			CxxTypeNameResolver typenNameResolver(this);
			typenNameResolver.ignoreContextDecl(functionDecl);
			std::unique_ptr<CxxTypeName> returnTypeName = CxxTypeName::makeUnsolvedIfNull(
				typenNameResolver.getName(functionDecl->getReturnType()));

			std::vector<std::unique_ptr<CxxTypeName>> parameterTypeNames;
			for (unsigned int i = 0; i < functionDecl->param_size(); i++)
			{
				if (const clang::SubstTemplateTypeParmType* substType =
						clang::dyn_cast_or_null<clang::SubstTemplateTypeParmType>(
							functionDecl->parameters()[i]->getType().getTypePtr()))
				{
					if (const clang::TemplateTypeParmType* templateParamType =
							substType->getReplacedParameter())
					{
						if (templateParamType->isParameterPack())
						{
							parameterTypeNames.push_back(std::make_unique<CxxTypeName>(L"..."));
							break;
						}
					}
				}
				parameterTypeNames.push_back(CxxTypeName::makeUnsolvedIfNull(
					typenNameResolver.getName(functionDecl->parameters()[i]->getType())));
			}

			if (!clang::isa<clang::CXXMethodDecl>(declaration) && isStatic)
			{
				return std::make_unique<CxxStaticFunctionDeclName>(
					std::move(functionName),
					std::move(templateArguments),
					std::move(returnTypeName),
					std::move(parameterTypeNames),
					getTranslationUnitMainFileName(declaration));
			}

			return std::make_unique<CxxFunctionDeclName>(
				std::move(functionName),
				std::move(templateArguments),
				std::move(returnTypeName),
				std::move(parameterTypeNames),
				isConst,
				isStatic);
		}
		else if (clang::isa<clang::FunctionTemplateDecl>(declaration))
		{
			const clang::FunctionTemplateDecl* functionTemplateDecl =
				clang::dyn_cast<clang::FunctionTemplateDecl>(declaration);
			return getDeclName(functionTemplateDecl->getTemplatedDecl());
		}
		else if (clang::isa<clang::FieldDecl>(declaration))
		{
			const clang::FieldDecl* fieldDecl = clang::dyn_cast<clang::FieldDecl>(declaration);
			CxxTypeNameResolver typenNameResolver(this);
			typenNameResolver.ignoreContextDecl(fieldDecl);
			std::unique_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(
				typenNameResolver.getName(fieldDecl->getType()));
			return std::make_unique<CxxVariableDeclName>(
				std::move(declNameString), std::vector<std::wstring>(), std::move(typeName), false);
		}
		else if (
			clang::isa<clang::NamespaceDecl>(declaration) &&
			clang::dyn_cast<clang::NamespaceDecl>(declaration)->isAnonymousNamespace())
		{
			declaration = clang::dyn_cast<clang::NamespaceDecl>(declaration)->getOriginalNamespace();
			return std::make_unique<CxxDeclName>(getNameForAnonymousSymbol(L"namespace", declaration));
		}
		else if (clang::isa<clang::EnumDecl>(declaration) && declNameString.empty())
		{
			return std::make_unique<CxxDeclName>(getNameForAnonymousSymbol(L"enum", declaration));
		}
		else if (
			(clang::isa<clang::TemplateTypeParmDecl>(declaration) ||
			 clang::isa<clang::NonTypeTemplateParmDecl>(declaration) ||
			 clang::isa<clang::TemplateTemplateParmDecl>(declaration)) &&
			declNameString.empty())
		{
			return std::make_unique<CxxDeclName>(
				getNameForAnonymousSymbol(L"template parameter", declaration));
		}
		else if (clang::isa<clang::ParmVarDecl>(declaration) && declNameString.empty())
		{
			return std::make_unique<CxxDeclName>(getNameForAnonymousSymbol(L"parameter", declaration));
		}
		else if (clang::isa<clang::VarDecl>(declaration))
		{
			const clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declaration);
			if (varDecl->getParentFunctionOrMethod() == nullptr)
			{
				bool isStatic = false;
				if (varDecl->getAccess() != clang::AS_none)
				{
					// var is declared inside a type and must be static (non-statics are stored as
					// clang::FieldDecl)
					isStatic = true;
				}
				else
				{
					// nothing todo, varDecl is global (and non-static)
				}

				CxxTypeNameResolver typenNameResolver(this);
				typenNameResolver.ignoreContextDecl(varDecl);
				std::unique_ptr<CxxTypeName> typeName = CxxTypeName::makeUnsolvedIfNull(
					typenNameResolver.getName(varDecl->getType()));

				std::wstring varName = declNameString;
				if (utility::getSymbolKind(varDecl) == SYMBOL_GLOBAL_VARIABLE &&
					varDecl->getStorageClass() == clang::SC_Static)
				{
					// if a global variable is static it is only visible in the current translation
					// unit. Therefore if multiple instances of that global variable may be
					// generated (one for each translation unit) we add the name of the translation
					// unit's source file. If that global variable definition is const, we add the
					// name of the (maybe header) file that variable is defined in instead. This
					// causes different instances of the variable that all MUST contain the same
					// value to be merged into a single node in Sourcetrail.
					std::wstring scopeFileName;
					if (varDecl->getType().isConstQualified())
					{
						scopeFileName = getCanonicalFilePathCache()->getDeclarationFileName(
							declaration);
					}
					else
					{
						scopeFileName = getTranslationUnitMainFileName(declaration);
					}

					if (!scopeFileName.empty())
					{
						varName = declNameString + L" (" + scopeFileName + L')';
					}
				}

				std::vector<std::wstring> templateParameterNames;
				if (varDecl->getDescribedVarTemplate())
				{
					const clang::VarTemplateDecl* templateDeclaration =
						varDecl->getDescribedVarTemplate();
					templateParameterNames = getTemplateParameterStrings(templateDeclaration);
				}
				else if (clang::isa<clang::VarTemplatePartialSpecializationDecl>(declaration))
				{
					templateParameterNames = getTemplateParameterStringsOfPartialSpecialization(
						clang::dyn_cast<clang::VarTemplatePartialSpecializationDecl>(declaration));
				}
				else if (clang::isa<clang::VarTemplateSpecializationDecl>(declaration))
				{
					const clang::VarTemplateSpecializationDecl* templateSpecializationDeclaration =
						clang::dyn_cast_or_null<clang::VarTemplateSpecializationDecl>(varDecl);
					const clang::TemplateArgumentList& templateArgumentList =
						templateSpecializationDeclaration->getTemplateArgs();
					for (unsigned i = 0; i < templateArgumentList.size(); i++)
					{
						const clang::TemplateArgument& templateArgument = templateArgumentList.get(i);
						if (templateArgument.isDependent())
						{
							llvm::PointerUnion<clang::VarTemplateDecl*, clang::VarTemplatePartialSpecializationDecl*>
								pu = templateSpecializationDeclaration->getSpecializedTemplateOrPartial();
							if (pu.is<clang::VarTemplateDecl*>())
							{
								return getDeclName(pu.get<clang::VarTemplateDecl*>());
							}
							else if (pu.is<clang::VarTemplatePartialSpecializationDecl*>())
							{
								return getDeclName(
									pu.get<clang::VarTemplatePartialSpecializationDecl*>());
							}
						}
						templateParameterNames.push_back(getTemplateArgumentName(templateArgument));
					}
				}

				return std::make_unique<CxxVariableDeclName>(
					std::move(varName),
					std::move(templateParameterNames),
					std::move(typeName),
					isStatic);
			}
		}
		else if (clang::isa<clang::VarTemplateDecl>(declaration))
		{
			const clang::VarTemplateDecl* varTemplateDecl = clang::dyn_cast<clang::VarTemplateDecl>(
				declaration);
			return getDeclName(varTemplateDecl->getTemplatedDecl());
		}
		else if (clang::isa<clang::TemplateDecl>(
					 declaration))	  // also triggers on TemplateTemplateParmDecl
		{
			return std::make_unique<CxxDeclName>(
				std::move(declNameString),
				getTemplateParameterStrings(clang::dyn_cast<clang::TemplateDecl>(declaration)));
		}

		if (!declNameString.empty())
		{
			return std::make_unique<CxxDeclName>(std::move(declNameString));
		}
	}

	// LOG_ERROR("could not resolve name of decl at: " +
	// declaration->getLocation().printToString(sourceManager));
	return std::make_unique<CxxDeclName>(getNameForAnonymousSymbol(L"symbol", declaration));
}

std::wstring CxxDeclNameResolver::getTranslationUnitMainFileName(const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	return getCanonicalFilePathCache()
		->getCanonicalFilePath(sourceManager.getMainFileID(), sourceManager)
		.fileName();
}

std::wstring CxxDeclNameResolver::getNameForAnonymousSymbol(
	const std::wstring& symbolKindName, const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(declaration->getBeginLoc());

	if (presumedBegin.isValid())
	{
		return L"anonymous " + symbolKindName + L" (" +
			getCanonicalFilePathCache()->getDeclarationFileName(declaration) + L'<' +
			std::to_wstring(presumedBegin.getLine()) + L':' +
			std::to_wstring(presumedBegin.getColumn()) + L">)";
	}
	return L"anonymous " + symbolKindName;
}

std::vector<std::wstring> CxxDeclNameResolver::getTemplateParameterStrings(
	const clang::TemplateDecl* templateDecl)
{
	std::vector<std::wstring> templateParameterStrings;
	clang::TemplateParameterList* parameterList = templateDecl->getTemplateParameters();
	for (unsigned i = 0; i < parameterList->size(); i++)
	{
		templateParameterStrings.push_back(getTemplateParameterString(parameterList->getParam(i)));
	}
	return templateParameterStrings;
}

std::wstring CxxDeclNameResolver::getTemplateParameterString(const clang::NamedDecl* parameter)
{
	CxxTemplateParameterStringResolver parameterStringResolver(this);
	if (clang::isa<clang::TemplateDecl>(m_currentDecl) &&
		clang::dyn_cast<clang::TemplateDecl>(m_currentDecl)->getTemplatedDecl())
	{
		parameterStringResolver.ignoreContextDecl(
			clang::dyn_cast<clang::TemplateDecl>(m_currentDecl)->getTemplatedDecl());
	}
	else	// works for partial template specializations
	{
		parameterStringResolver.ignoreContextDecl(m_currentDecl);
	}
	parameterStringResolver.ignoreContextDecl(parameter);

	return parameterStringResolver.getTemplateParameterString(parameter);
}

std::wstring CxxDeclNameResolver::getTemplateArgumentName(const clang::TemplateArgument& argument)
{
	return CxxTemplateArgumentNameResolver(this).getTemplateArgumentName(argument);
}
