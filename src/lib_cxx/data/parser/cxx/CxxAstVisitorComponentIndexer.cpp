#include "CxxAstVisitorComponentIndexer.h"

#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Preprocessor.h>

#include "CanonicalFilePathCache.h"
#include "CxxAstVisitor.h"
#include "CxxAstVisitorComponentContext.h"
#include "CxxAstVisitorComponentDeclRefKind.h"
#include "CxxAstVisitorComponentTypeRefKind.h"
#include "CxxDeclNameResolver.h"
#include "CxxFunctionDeclName.h"
#include "CxxTypeNameResolver.h"
#include "ParserClient.h"
#include "utilityClang.h"

CxxAstVisitorComponentIndexer::CxxAstVisitorComponentIndexer(
	CxxAstVisitor* astVisitor, clang::ASTContext* astContext, std::shared_ptr<ParserClient> client)
	: CxxAstVisitorComponent(astVisitor), m_astContext(astContext), m_client(client)
{
}

void CxxAstVisitorComponentIndexer::beginTraverseNestedNameSpecifierLoc(
	const clang::NestedNameSpecifierLoc& loc)
{
	if (!getAstVisitor()->shouldVisitReference(loc.getBeginLoc()))
	{
		return;
	}

	switch (loc.getNestedNameSpecifier()->getKind())
	{
	case clang::NestedNameSpecifier::Identifier:
		break;
	case clang::NestedNameSpecifier::Namespace:
	{
		Id symbolId = getOrCreateSymbolId(loc.getNestedNameSpecifier()->getAsNamespace());
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);
		m_client->recordLocation(
			symbolId, getParseLocation(loc.getLocalBeginLoc()), ParseLocationType::QUALIFIER);
	}
	break;
	case clang::NestedNameSpecifier::NamespaceAlias:
	{
		Id symbolId = getOrCreateSymbolId(loc.getNestedNameSpecifier()->getAsNamespaceAlias());
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);

		symbolId = getOrCreateSymbolId(
			loc.getNestedNameSpecifier()->getAsNamespaceAlias()->getAliasedNamespace());
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);
	}
	break;
	case clang::NestedNameSpecifier::Global:
	case clang::NestedNameSpecifier::Super:
		break;
	case clang::NestedNameSpecifier::TypeSpec:
	case clang::NestedNameSpecifier::TypeSpecWithTemplate:
		if (const clang::CXXRecordDecl* recordDecl = loc.getNestedNameSpecifier()->getAsRecordDecl())
		{
			SymbolKind symbolKind = SYMBOL_KIND_MAX;
			if (recordDecl->isClass())
			{
				symbolKind = SYMBOL_CLASS;
			}
			else if (recordDecl->isStruct())
			{
				symbolKind = SYMBOL_STRUCT;
			}
			else if (recordDecl->isUnion())
			{
				symbolKind = SYMBOL_UNION;
			}

			if (symbolKind != SYMBOL_KIND_MAX)
			{
				const Id symbolId = getOrCreateSymbolId(recordDecl);
				m_client->recordSymbolKind(symbolId, symbolKind);
				m_client->recordLocation(
					symbolId, getParseLocation(loc.getLocalBeginLoc()), ParseLocationType::QUALIFIER);
			}
		}
		else if (const clang::Type* type = loc.getNestedNameSpecifier()->getAsType())
		{
			const ParseLocation parseLocation = getParseLocation(loc.getLocalBeginLoc());

			if (const clang::TemplateTypeParmType* tpt =
					clang::dyn_cast_or_null<clang::TemplateTypeParmType>(type))
			{
				clang::TemplateTypeParmDecl* d = tpt->getDecl();
				if (d)
				{
					m_client->recordLocalSymbol(getLocalSymbolName(d->getLocation()), parseLocation);
				}
			}
			else
			{
				const Id symbolId = getOrCreateSymbolId(type);
				m_client->recordLocation(symbolId, parseLocation, ParseLocationType::QUALIFIER);
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::beginTraverseTemplateArgumentLoc(
	const clang::TemplateArgumentLoc& loc)
{
	if (getAstVisitor()->shouldVisitReference(loc.getLocation()))
	{
		if (loc.getArgument().getKind() == clang::TemplateArgument::Template)
		{
			// TODO: maybe move this to VisitTemplateName

			const clang::TemplateName templateTemplateArgumentName = loc.getArgument().getAsTemplate();

			const ParseLocation parseLocation = getParseLocation(loc.getLocation());
			if (templateTemplateArgumentName.isDependent())
			{
				clang::SourceLocation declLocation;
				if (templateTemplateArgumentName.getAsTemplateDecl())
				{
					declLocation = templateTemplateArgumentName.getAsTemplateDecl()->getLocation();
				}
				else
				{
					declLocation = loc.getLocation();
				}
				m_client->recordLocalSymbol(getLocalSymbolName(declLocation), parseLocation);
			}
			else
			{
				const Id symbolId = getOrCreateSymbolId(
					templateTemplateArgumentName.getAsTemplateDecl());

				m_client->recordReference(
					REFERENCE_TYPE_USAGE,
					symbolId,
					getOrCreateSymbolId(
						getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
					parseLocation);

				{
					if (const clang::NamedDecl* namedContextDecl =
							getAstVisitor()
								->getComponent<CxxAstVisitorComponentContext>()
								->getTopmostContextDecl(1))
					{
						m_client->recordReference(
							REFERENCE_TYPE_USAGE,
							symbolId,
							getOrCreateSymbolId(namedContextDecl),	  // we use the closest named decl
																	  // here (e.g. function decl)
							parseLocation);
					}
				}
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::beginTraverseLambdaCapture(
	clang::LambdaExpr* lambdaExpr, const clang::LambdaCapture* capture)
{
	if ((!lambdaExpr->isInitCapture(capture)) && (capture->capturesVariable()))
	{
		clang::VarDecl* d = capture->getCapturedVar();
		if (utility::isLocalVariable(d) || utility::isParameter(d))
		{
			if (!d->getNameAsString().empty())	  // don't record anonymous parameters
			{
				m_client->recordLocalSymbol(
					getLocalSymbolName(d->getLocation()), getParseLocation(capture->getLocation()));
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitTagDecl(clang::TagDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		DefinitionKind definitionKind = DEFINITION_NONE;
		if (d->isThisDeclarationADefinition())
		{
			definitionKind = utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT;
		}

		const SymbolKind symbolKind = utility::convertTagKind(d->getTagKind());
		const ParseLocation location = getParseLocation(d->getLocation());

		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(symbolId, symbolKind);
		m_client->recordLocation(symbolId, location, ParseLocationType::TOKEN);
		m_client->recordLocation(
			symbolId, getParseLocationOfTagDeclBody(d), ParseLocationType::SCOPE);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(symbolId, definitionKind);

		if (clang::EnumDecl* enumDecl = clang::dyn_cast_or_null<clang::EnumDecl>(d))
		{
			recordTemplateMemberSpecialization(
				enumDecl->getMemberSpecializationInfo(), symbolId, location, symbolKind);
		}

		if (clang::CXXRecordDecl* recordDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(d))
		{
			recordTemplateMemberSpecialization(
				recordDecl->getMemberSpecializationInfo(), symbolId, location, symbolKind);
		}
	}
}

void CxxAstVisitorComponentIndexer::visitClassTemplateSpecializationDecl(
	clang::ClassTemplateSpecializationDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		clang::CXXRecordDecl* specializedFromDecl = nullptr;

		llvm::PointerUnion<clang::ClassTemplateDecl*, clang::ClassTemplatePartialSpecializationDecl*> pu =
			d->getSpecializedTemplateOrPartial();
		if (pu.is<clang::ClassTemplateDecl*>())
		{
			specializedFromDecl = pu.get<clang::ClassTemplateDecl*>()->getTemplatedDecl();
		}
		else if (pu.is<clang::ClassTemplatePartialSpecializationDecl*>())
		{
			specializedFromDecl = pu.get<clang::ClassTemplatePartialSpecializationDecl*>();
		}

		m_client->recordReference(
			REFERENCE_TEMPLATE_SPECIALIZATION,
			getOrCreateSymbolId(specializedFromDecl),
			getOrCreateSymbolId(d),
			getParseLocation(d->getLocation()));
	}
}

void CxxAstVisitorComponentIndexer::visitVarDecl(clang::VarDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		if (utility::isLocalVariable(d) || utility::isParameter(d))
		{
			if (!d->getNameAsString().empty())	  // don't record anonymous parameters
			{
				m_client->recordLocalSymbol(
					getLocalSymbolName(d->getLocation()), getParseLocation(d->getLocation()));
			}
		}
		else
		{
			const SymbolKind symbolKind = utility::getSymbolKind(d);
			const ParseLocation location = getParseLocation(d->getLocation());

			Id symbolId = getOrCreateSymbolId(d);
			m_client->recordSymbolKind(symbolId, symbolKind);
			m_client->recordLocation(symbolId, location, ParseLocationType::TOKEN);
			m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
			m_client->recordDefinitionKind(
				symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);

			recordTemplateMemberSpecialization(
				d->getMemberSpecializationInfo(), symbolId, location, symbolKind);
		}
	}
}

void CxxAstVisitorComponentIndexer::visitVarTemplateSpecializationDecl(
	clang::VarTemplateSpecializationDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		clang::NamedDecl* specializedFromDecl = nullptr;

		// todo: use context and childcontext!!
		llvm::PointerUnion<clang::VarTemplateDecl*, clang::VarTemplatePartialSpecializationDecl*> pu =
			d->getSpecializedTemplateOrPartial();
		if (pu.is<clang::VarTemplateDecl*>())
		{
			specializedFromDecl = pu.get<clang::VarTemplateDecl*>();
		}
		else if (pu.is<clang::VarTemplatePartialSpecializationDecl*>())
		{
			specializedFromDecl = pu.get<clang::VarTemplatePartialSpecializationDecl*>();
		}

		m_client->recordReference(
			REFERENCE_TEMPLATE_SPECIALIZATION,
			getOrCreateSymbolId(specializedFromDecl),
			getOrCreateSymbolId(d),
			getParseLocation(d->getLocation()));
	}
}

void CxxAstVisitorComponentIndexer::visitFieldDecl(clang::FieldDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		if (clang::isa<clang::ObjCIvarDecl>(d))
		{
			return;
		}

		const ParseLocation location = getParseLocation(d->getLocation());

		Id fieldId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(fieldId, SYMBOL_FIELD);
		m_client->recordLocation(fieldId, location, ParseLocationType::TOKEN);
		m_client->recordAccessKind(fieldId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			fieldId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);

		if (clang::CXXRecordDecl* declaringRecordDecl =
				clang::dyn_cast_or_null<clang::CXXRecordDecl>(d->getParent()))
		{
			if (clang::CXXRecordDecl* declaringRecordTemplateDecl =
					declaringRecordDecl->getTemplateInstantiationPattern())
			{
				for (clang::FieldDecl* templateFieldDecl: declaringRecordTemplateDecl->fields())
				{
					if (d->getName() == templateFieldDecl->getName())
					{
						Id templateFieldId = getOrCreateSymbolId(templateFieldDecl);
						m_client->recordSymbolKind(templateFieldId, SYMBOL_FIELD);
						m_client->recordReference(
							REFERENCE_TEMPLATE_SPECIALIZATION, templateFieldId, fieldId, location);
						break;
					}
				}
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitFunctionDecl(clang::FunctionDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(
			symbolId, clang::isa<clang::CXXMethodDecl>(d) ? SYMBOL_METHOD : SYMBOL_FUNCTION);
		m_client->recordLocation(
			symbolId, getParseLocation(d->getNameInfo().getSourceRange()), ParseLocationType::TOKEN);
		m_client->recordLocation(
			symbolId, getParseLocationOfFunctionBody(d), ParseLocationType::SCOPE);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);

		if (d->isFirstDecl())
		{
			m_client->recordLocation(symbolId, getSignatureLocation(d), ParseLocationType::SIGNATURE);
		}

		if (d->isFunctionTemplateSpecialization())
		{
			if (clang::isa<clang::ClassTemplateSpecializationDecl>(d->getParent()) &&
				!clang::isa<clang::ClassTemplatePartialSpecializationDecl>(d->getParent()) &&
				!clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(d->getParent())
					 ->isExplicitSpecialization())
			{
				// record edge from Foo<int>::bar<float>() to Foo<T>::bar<U>() instead of recording
				// an edge from Foo<int>::bar<float>() to Foo<int>::bar<U>() because there is not
				// "written" code for Foo<int>::bar<U>() if Foo<int> is an implicit template
				// specialization.
				if (clang::CXXRecordDecl* declaringRecordDecl =
						clang::dyn_cast_or_null<clang::CXXRecordDecl>(d->getParent()))
				{
					if (clang::CXXRecordDecl* declaringRecordTemplateDecl =
							declaringRecordDecl->getTemplateInstantiationPattern())
					{
						for (clang::Decl* templateMethodDecl: declaringRecordTemplateDecl->decls())
						{
							if (clang::FunctionTemplateDecl* functionTemplateDecl =
									clang::dyn_cast_or_null<clang::FunctionTemplateDecl>(
										templateMethodDecl))
							{
								if (d->getName() == functionTemplateDecl->getName())
								{
									Id templateMethodId = getOrCreateSymbolId(functionTemplateDecl);
									m_client->recordSymbolKind(templateMethodId, SYMBOL_METHOD);
									m_client->recordReference(
										REFERENCE_TEMPLATE_SPECIALIZATION,
										templateMethodId,
										symbolId,
										getParseLocation(d->getLocation()));
									break;
								}
							}
						}
					}
				}
			}
			else
			{
				// record edge from foo<int>() to foo<T>()
				Id templateId = getOrCreateSymbolId(d->getPrimaryTemplate()->getTemplatedDecl());
				m_client->recordSymbolKind(templateId, SYMBOL_FUNCTION);
				m_client->recordReference(
					REFERENCE_TEMPLATE_SPECIALIZATION,
					templateId,
					symbolId,
					getParseLocation(d->getLocation()));
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitCXXMethodDecl(clang::CXXMethodDecl* d)
{
	// Decl has been recorded in VisitFunctionDecl
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		ParseLocation location = getParseLocation(d->getLocation());

		for (clang::CXXMethodDecl::method_iterator it =
				 d->begin_overridden_methods();	   // TODO: iterate in traversal and use
												   // REFERENCE_OVERRIDE or so..
			 it != d->end_overridden_methods();
			 it++)
		{
			Id overrideId = getOrCreateSymbolId(*it);
			m_client->recordSymbolKind(overrideId, SYMBOL_FUNCTION);
			m_client->recordReference(REFERENCE_OVERRIDE, overrideId, symbolId, location);
		}

		// record edge from Foo::bar<int>() to Foo::bar<T>()
		recordTemplateMemberSpecialization(
			d->getMemberSpecializationInfo(), symbolId, location, SYMBOL_FUNCTION);
	}
}

void CxxAstVisitorComponentIndexer::visitEnumConstantDecl(clang::EnumConstantDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(symbolId, SYMBOL_ENUM_CONSTANT);
		m_client->recordLocation(
			symbolId, getParseLocation(d->getLocation()), ParseLocationType::TOKEN);
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);
	}
}

void CxxAstVisitorComponentIndexer::visitNamespaceDecl(clang::NamespaceDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);
		m_client->recordLocation(
			symbolId, getParseLocation(d->getLocation()), ParseLocationType::TOKEN);
		m_client->recordLocation(
			symbolId, getParseLocation(d->getSourceRange()), ParseLocationType::SCOPE);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);
	}
}

void CxxAstVisitorComponentIndexer::visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);
		m_client->recordLocation(
			symbolId, getParseLocation(d->getLocation()), ParseLocationType::TOKEN);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);

		m_client->recordReference(
			REFERENCE_USAGE,
			getOrCreateSymbolId(d->getAliasedNamespace()),
			symbolId,
			getParseLocation(d->getTargetNameLoc()));

		// TODO: record other namespace as undefined
	}
}

void CxxAstVisitorComponentIndexer::visitTypedefDecl(clang::TypedefDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(
			symbolId,
			d->getAnonDeclWithTypedefName() == nullptr
				? SYMBOL_TYPEDEF
				: utility::convertTagKind(d->getAnonDeclWithTypedefName()->getTagKind()));
		m_client->recordLocation(
			symbolId, getParseLocation(d->getLocation()), ParseLocationType::TOKEN);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);
	}
}

void CxxAstVisitorComponentIndexer::visitTypeAliasDecl(clang::TypeAliasDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d);
		m_client->recordSymbolKind(
			symbolId,
			d->getAnonDeclWithTypedefName() == nullptr
				? SYMBOL_TYPEDEF
				: utility::convertTagKind(d->getAnonDeclWithTypedefName()->getTagKind()));
		m_client->recordLocation(
			symbolId, getParseLocation(d->getLocation()), ParseLocationType::TOKEN);
		m_client->recordAccessKind(symbolId, utility::convertAccessSpecifier(d->getAccess()));
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);
	}
}

void CxxAstVisitorComponentIndexer::visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		Id symbolId = getOrCreateSymbolId(d->getNominatedNamespaceAsWritten());
		m_client->recordSymbolKind(symbolId, SYMBOL_NAMESPACE);

		const ParseLocation location = getParseLocation(d->getLocation());

		m_client->recordReference(
			REFERENCE_USAGE,
			symbolId,
			getOrCreateSymbolId(
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext(),
				NameHierarchy(
					getAstVisitor()
						->getCanonicalFilePathCache()
						->getCanonicalFilePath(location.fileId)
						.wstr(),
					NAME_DELIMITER_FILE)),
			location);
	}
}

void CxxAstVisitorComponentIndexer::visitUsingDecl(clang::UsingDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d))
	{
		const ParseLocation location = getParseLocation(d->getLocation());

		m_client->recordReference(
			REFERENCE_USAGE,
			getOrCreateSymbolId(d),
			getOrCreateSymbolId(
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext(),
				NameHierarchy(
					getAstVisitor()
						->getCanonicalFilePathCache()
						->getCanonicalFilePath(location.fileId)
						.wstr(),
					NAME_DELIMITER_FILE)),
			location);
	}
}

void CxxAstVisitorComponentIndexer::visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d) &&
		!d->getName().empty())	  // We don't create symbols for unnamed template parameters.
	{
		m_client->recordLocalSymbol(
			getLocalSymbolName(d->getLocation()), getParseLocation(d->getLocation()));
	}
}

void CxxAstVisitorComponentIndexer::visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d) &&
		!d->getName().empty())	  // We don't create symbols for unnamed template parameters.
	{
		m_client->recordLocalSymbol(
			getLocalSymbolName(d->getLocation()), getParseLocation(d->getLocation()));
	}
}

void CxxAstVisitorComponentIndexer::visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	if (getAstVisitor()->shouldVisitDecl(d) &&
		!d->getName().empty())	  // We don't create symbols for unnamed template parameters.
	{
		m_client->recordLocalSymbol(
			getLocalSymbolName(d->getLocation()), getParseLocation(d->getLocation()));
	}
}

void CxxAstVisitorComponentIndexer::visitTypeLoc(clang::TypeLoc tl)
{
	clang::TypeLoc::TypeLocClass tlcc = tl.getTypeLocClass();
	if ((getAstVisitor()->shouldVisitReference(tl.getBeginLoc())) &&
		(!getAstVisitor()->checkIgnoresTypeLoc(tl)))
	{
		clang::TypeLoc::TypeLocClass tlcc = tl.getTypeLocClass();
		if (!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull())
		{
			const clang::TemplateTypeParmTypeLoc& ttptl = tl.castAs<clang::TemplateTypeParmTypeLoc>();
			clang::TemplateTypeParmDecl* d = ttptl.getDecl();
			m_client->recordLocalSymbol(
				getLocalSymbolName(d->getLocation()), getParseLocation(tl.getBeginLoc()));
		}
		else
		{
			if (!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull())
			{
				const clang::TemplateSpecializationTypeLoc& tstl =
					tl.castAs<clang::TemplateSpecializationTypeLoc>();
				const clang::TemplateSpecializationType* tst = tstl.getTypePtr();
				const clang::TemplateName tln = tst->getTemplateName();
				clang::TemplateName::NameKind nknk = tln.getKind();
				if (tln.isDependent())	  // e.g. T<int> where the template name T depends on a
										  // template parameter
				{
					clang::TemplateDecl* d = tst->getTemplateName().getAsTemplateDecl();
					m_client->recordLocalSymbol(
						getLocalSymbolName(d->getLocation()), getParseLocation(tl.getBeginLoc()));
					return;
				}
			}

			Id symbolId = getOrCreateSymbolId(tl.getTypePtr());

			if (clang::dyn_cast_or_null<clang::BuiltinType>(tl.getTypePtr()))
			{
				m_client->recordSymbolKind(symbolId, SYMBOL_BUILTIN_TYPE);
				m_client->recordDefinitionKind(symbolId, DEFINITION_EXPLICIT);
			}

			clang::SourceLocation loc;
			if (!tl.getAs<clang::DependentNameTypeLoc>().isNull())
			{
				const clang::DependentNameTypeLoc& dntl = tl.castAs<clang::DependentNameTypeLoc>();
				loc = dntl.getNameLoc();
			}
			else
			{
				loc = tl.getBeginLoc();
			}

			const ParseLocation parseLocation = getParseLocation(loc);

			m_client->recordReference(
				getAstVisitor()->getComponent<CxxAstVisitorComponentTypeRefKind>()->isTraversingInheritance()
					? REFERENCE_INHERITANCE
					: REFERENCE_TYPE_USAGE,
				symbolId,
				getOrCreateSymbolId(
					getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext(
						1)),	// we skip the last element because it refers to this typeloc.
				parseLocation);

			if (getAstVisitor()
					->getComponent<CxxAstVisitorComponentTypeRefKind>()
					->isTraversingTemplateArgument())
			{
				if (const clang::NamedDecl* namedContextDecl =
						getAstVisitor()
							->getComponent<CxxAstVisitorComponentContext>()
							->getTopmostContextDecl(2))
				{
					m_client->recordReference(
						REFERENCE_TYPE_USAGE,
						symbolId,
						getOrCreateSymbolId(namedContextDecl),	  // we use the closest named decl here
						parseLocation);
				}
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitDeclRefExpr(clang::DeclRefExpr* s)
{
	clang::ValueDecl* decl = s->getDecl();
	if (getAstVisitor()->shouldVisitReference(s->getLocation()))
	{
		if ((clang::isa<clang::ParmVarDecl>(decl)) ||
			(clang::isa<clang::VarDecl>(decl) && decl->getParentFunctionOrMethod() != nullptr))
		{
			if (!utility::isImplicit(decl))
			{
				m_client->recordLocalSymbol(
					getLocalSymbolName(decl->getLocation()), getParseLocation(s->getLocation()));
			}
			// else { don't do anything }
		}
		else if (
			(clang::isa<clang::NonTypeTemplateParmDecl>(decl)) ||
			(clang::isa<clang::TemplateTypeParmDecl>(decl)) ||
			(clang::isa<clang::TemplateTemplateParmDecl>(decl)))
		{
			m_client->recordLocalSymbol(
				getLocalSymbolName(decl->getLocation()), getParseLocation(s->getLocation()));
		}
		else
		{
			Id symbolId = getOrCreateSymbolId(decl);

			const ReferenceKind refKind = consumeDeclRefContextKind();
			if (refKind == REFERENCE_CALL)
			{
				m_client->recordSymbolKind(symbolId, SYMBOL_FUNCTION);
			}

			m_client->recordReference(
				refKind,
				symbolId,
				getOrCreateSymbolId(
					getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
				getParseLocation(s->getLocation()));
		}
	}
}

void CxxAstVisitorComponentIndexer::visitMemberExpr(clang::MemberExpr* s)
{
	if (getAstVisitor()->shouldVisitReference(s->getMemberLoc()))
	{
		Id symbolId = getOrCreateSymbolId(s->getMemberDecl());

		const ReferenceKind refKind = consumeDeclRefContextKind();
		if (refKind == REFERENCE_CALL)
		{
			m_client->recordSymbolKind(symbolId, SYMBOL_FUNCTION);
		}

		m_client->recordReference(
			refKind,
			symbolId,
			getOrCreateSymbolId(
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
			getParseLocation(s->getMemberLoc()));
	}
}

void CxxAstVisitorComponentIndexer::visitCXXConstructExpr(clang::CXXConstructExpr* s)
{
	const clang::CXXConstructorDecl* constructorDecl = s->getConstructor();

	if (!constructorDecl)
	{
		return;
	}
	else
	{
		const clang::CXXRecordDecl* parentDecl = constructorDecl->getParent();
		if (!parentDecl || parentDecl->isLambda())
		{
			return;
		}
	}

	if (getAstVisitor()->shouldVisitReference(s->getLocation()))
	{
		clang::SourceLocation loc;
		clang::SourceLocation braceBeginLoc = s->getParenOrBraceRange().getBegin();
		clang::SourceLocation nameBeginLoc = s->getSourceRange().getBegin();
		if (braceBeginLoc.isValid())
		{
			if (braceBeginLoc == nameBeginLoc)
			{
				loc = nameBeginLoc;
			}
			else
			{
				loc = braceBeginLoc.getLocWithOffset(-1);
			}
		}
		else
		{
			loc = s->getSourceRange().getEnd();
		}
		loc = clang::Lexer::GetBeginningOfToken(
			loc, m_astContext->getSourceManager(), m_astContext->getLangOpts());

		const Id symbolId = getOrCreateSymbolId(s->getConstructor());

		const ReferenceKind refKind = consumeDeclRefContextKind();
		if (refKind == REFERENCE_CALL)
		{
			m_client->recordSymbolKind(symbolId, SYMBOL_FUNCTION);
		}

		m_client->recordReference(
			refKind,
			symbolId,
			getOrCreateSymbolId(
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
			getParseLocation(loc));
	}
}

void CxxAstVisitorComponentIndexer::visitCXXDeleteExpr(clang::CXXDeleteExpr* s)
{
	if (!s->isArrayForm() && getAstVisitor()->shouldVisitReference(s->getBeginLoc()))
	{
		if (clang::CXXRecordDecl* recordDecl = s->getDestroyedType()->getAsCXXRecordDecl())
		{
			if (clang::CXXDestructorDecl* destructorDecl = recordDecl->getDestructor())
			{
				const Id symbolId = getOrCreateSymbolId(destructorDecl);

				m_client->recordReference(
					REFERENCE_CALL,
					symbolId,
					getOrCreateSymbolId(
						getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
					getParseLocation(s->getBeginLoc()));
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitLambdaExpr(clang::LambdaExpr* s)
{
	clang::CXXMethodDecl* methodDecl = s->getCallOperator();
	if (getAstVisitor()->shouldVisitDecl(methodDecl))
	{
		Id symbolId = getOrCreateSymbolId(methodDecl);
		m_client->recordSymbolKind(symbolId, SYMBOL_FUNCTION);
		m_client->recordLocation(
			symbolId, getParseLocation(s->getBeginLoc()), ParseLocationType::TOKEN);
		m_client->recordLocation(
			symbolId, getParseLocationOfFunctionBody(methodDecl), ParseLocationType::SCOPE);
		m_client->recordDefinitionKind(
			symbolId, utility::isImplicit(methodDecl) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT);
	}
}

void CxxAstVisitorComponentIndexer::visitConstructorInitializer(clang::CXXCtorInitializer* init)
{
	if (getAstVisitor()->shouldVisitReference(init->getMemberLocation()))
	{
		// record the field usage here because it is not a DeclRefExpr
		if (clang::FieldDecl* memberDecl = init->getMember())
		{
			m_client->recordReference(
				REFERENCE_USAGE,
				getOrCreateSymbolId(memberDecl),
				getOrCreateSymbolId(
					getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContext()),
				getParseLocation(init->getMemberLocation()));
		}
	}
}

void CxxAstVisitorComponentIndexer::recordTemplateMemberSpecialization(
	const clang::MemberSpecializationInfo* memberSpecializationInfo,
	Id contextId,
	const ParseLocation& location,
	SymbolKind symbolKind)
{
	if (memberSpecializationInfo != nullptr)
	{
		Id symbolId = getOrCreateSymbolId(memberSpecializationInfo->getInstantiatedFrom());
		m_client->recordSymbolKind(symbolId, symbolKind);
		m_client->recordReference(REFERENCE_TEMPLATE_SPECIALIZATION, symbolId, contextId, location);
	}
}

ParseLocation CxxAstVisitorComponentIndexer::getSignatureLocation(clang::FunctionDecl* d)
{
	clang::SourceRange signatureRange = d->getSourceRange();

	if (d->doesThisDeclarationHaveABody())
	{
		if (!d->getTypeSourceInfo())
		{
			return ParseLocation();
		}

		const clang::SourceManager& sm = m_astContext->getSourceManager();
		const clang::LangOptions& opts = m_astContext->getLangOpts();

		clang::SourceLocation endLoc = signatureRange.getBegin();

		if (d->getNumParams() > 0)
		{
			endLoc = d->getParamDecl(d->getNumParams() - 1)->getEndLoc();
		}

		while (sm.isBeforeInTranslationUnit(endLoc, signatureRange.getEnd()))
		{
			llvm::Optional<clang::Token> token = clang::Lexer::findNextToken(endLoc, sm, opts);
			if (token.hasValue())
			{
				const clang::tok::TokenKind tokenKind = token.getValue().getKind();
				if (tokenKind == clang::tok::l_brace || tokenKind == clang::tok::colon)
				{
					signatureRange.setEnd(endLoc);
					return getParseLocation(signatureRange);
				}

				clang::SourceLocation nextEndLoc = token.getValue().getLocation();
				if (nextEndLoc == endLoc)
				{
					return ParseLocation();
				}

				endLoc = nextEndLoc;
			}
			else
			{
				return ParseLocation();
			}
		}
		return ParseLocation();
	}

	return getParseLocation(signatureRange);
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocationOfTagDeclBody(clang::TagDecl* decl) const
{
	return getAstVisitor()->getParseLocationOfTagDeclBody(decl);
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocationOfFunctionBody(
	const clang::FunctionDecl* decl) const
{
	return getAstVisitor()->getParseLocationOfFunctionBody(decl);
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocation(const clang::SourceLocation& loc) const
{
	return getAstVisitor()->getParseLocation(loc);
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocation(const clang::SourceRange& sourceRange) const
{
	return getAstVisitor()->getParseLocation(sourceRange);
}

std::wstring CxxAstVisitorComponentIndexer::getLocalSymbolName(const clang::SourceLocation& loc) const
{
	const ParseLocation location = getParseLocation(loc);
	return getAstVisitor()->getCanonicalFilePathCache()->getCanonicalFilePath(location.fileId).fileName() +
		L"<" + std::to_wstring(location.startLineNumber) + L":" +
		std::to_wstring(location.startColumnNumber) + L">";
}

ReferenceKind CxxAstVisitorComponentIndexer::consumeDeclRefContextKind()
{
	ReferenceKind refKind = REFERENCE_UNDEFINED;

	CxxAstVisitorComponentTypeRefKind* typeRefKindComponent =
		getAstVisitor()->getComponent<CxxAstVisitorComponentTypeRefKind>();
	if (typeRefKindComponent->isTraversingInheritance())
	{
		return REFERENCE_INHERITANCE;
	}
	else if (typeRefKindComponent->isTraversingTemplateArgument())
	{
		return REFERENCE_TYPE_USAGE;
	}

	return getAstVisitor()->getComponent<CxxAstVisitorComponentDeclRefKind>()->getReferenceKind();
}

Id CxxAstVisitorComponentIndexer::getOrCreateSymbolId(const clang::NamedDecl* decl)
{
	auto it = m_declSymbolIds.find(decl);
	if (it != m_declSymbolIds.end())
	{
		return it->second;
	}

	NameHierarchy symbolName(L"global", NAME_DELIMITER_UNKNOWN);
	if (decl)
	{
		std::unique_ptr<CxxDeclName> declName =
			CxxDeclNameResolver(getAstVisitor()->getCanonicalFilePathCache()).getName(decl);
		if (declName)
		{
			symbolName = declName->toNameHierarchy();

			// TODO: replace duplicate main definition fix with better solution
			if (dynamic_cast<CxxFunctionDeclName*>(declName.get()) && symbolName.size() == 1 &&
				symbolName.back().getName() == L"main")
			{
				NameElement::Signature sig = symbolName.back().getSignature();
				symbolName.pop();
				symbolName.push(NameElement(
					L".:main:." +
						getAstVisitor()->getCanonicalFilePathCache()->getDeclarationFileName(decl),
					sig.getPrefix(),
					sig.getPostfix()));
			}
		}
	}

	Id symbolId = m_client->recordSymbol(symbolName);
	m_declSymbolIds.emplace(decl, symbolId);
	return symbolId;
}

Id CxxAstVisitorComponentIndexer::getOrCreateSymbolId(const clang::Type* type)
{
	auto it = m_typeSymbolIds.find(type);
	if (it != m_typeSymbolIds.end())
	{
		return it->second;
	}

	NameHierarchy symbolName(L"global", NAME_DELIMITER_UNKNOWN);
	if (type)
	{
		std::unique_ptr<CxxTypeName> typeName =
			CxxTypeNameResolver(getAstVisitor()->getCanonicalFilePathCache()).getName(type);
		if (typeName)
		{
			symbolName = typeName->toNameHierarchy();
		}
	}

	Id symbolId = m_client->recordSymbol(symbolName);
	m_typeSymbolIds.emplace(type, symbolId);
	return symbolId;
}

Id CxxAstVisitorComponentIndexer::getOrCreateSymbolId(const CxxContext* context)
{
	if (context)
	{
		if (context->getDecl())
		{
			return getOrCreateSymbolId(context->getDecl());
		}
		else
		{
			return getOrCreateSymbolId(context->getType());
		}
	}

	const clang::NamedDecl* decl {nullptr};
	return getOrCreateSymbolId(decl);
}

Id CxxAstVisitorComponentIndexer::getOrCreateSymbolId(
	const CxxContext* context, const NameHierarchy& fallback)
{
	if (context)
	{
		if (context->getDecl())
		{
			return getOrCreateSymbolId(context->getDecl());
		}
		else if (context->getType())
		{
			return getOrCreateSymbolId(context->getType());
		}
	}

	return m_client->recordSymbol(fallback);	// TODO: cache result somehow
}
