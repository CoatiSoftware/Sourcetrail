#include "data/parser/cxx/CxxAstVisitorComponentIndexer.h"

#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Preprocessor.h>

#include "data/parser/cxx/CxxAstVisitorComponentContext.h"
#include "data/parser/cxx/CxxAstVisitorComponentDeclRefKind.h"
#include "data/parser/cxx/CxxAstVisitorComponentTypeRefKind.h"

#include "data/parser/cxx/utilityCxxAstVisitor.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"

CxxAstVisitorComponentIndexer::CxxAstVisitorComponentIndexer(
	CxxAstVisitor* astVisitor, clang::ASTContext* astContext, std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister
)
	: CxxAstVisitorComponent(astVisitor)
	, m_astContext(astContext)
	, m_client(client)
	, m_fileRegister(fileRegister)
{
}

CxxAstVisitorComponentIndexer::~CxxAstVisitorComponentIndexer()
{
}

void CxxAstVisitorComponentIndexer::beginTraverseNestedNameSpecifierLoc(const clang::NestedNameSpecifierLoc& loc)
{
	if (!shouldVisitReference(loc.getBeginLoc(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	{
		return;
	}

	switch (loc.getNestedNameSpecifier()->getKind())
	{
	case clang::NestedNameSpecifier::Identifier:
		break;
	case clang::NestedNameSpecifier::Namespace:
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(loc.getNestedNameSpecifier()->getAsNamespace()),
			SYMBOL_NAMESPACE,
			ACCESS_NONE,
			DEFINITION_NONE
		);
		break;
	case clang::NestedNameSpecifier::NamespaceAlias:
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(loc.getNestedNameSpecifier()->getAsNamespaceAlias()),
			SYMBOL_NAMESPACE,
			ACCESS_NONE,
			DEFINITION_NONE
		);
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(loc.getNestedNameSpecifier()->getAsNamespaceAlias()->getAliasedNamespace()),
			SYMBOL_NAMESPACE,
			ACCESS_NONE,
			DEFINITION_NONE
		);
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
				m_client->recordSymbol(
					getAstVisitor()->getDeclNameCache()->getValue(recordDecl),
					symbolKind,
					ACCESS_NONE,
					DEFINITION_NONE
				);
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	if (
		(loc.getArgument().getKind() == clang::TemplateArgument::Template) &&
		(shouldVisitReference(loc.getLocation(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	){
		// TODO: maybe move this to VisitTemplateName
		m_client->recordReference(
			getAstVisitor()->getComponent<CxxAstVisitorComponentTypeRefKind>()->getReferenceKind(),
			getAstVisitor()->getDeclNameCache()->getValue(loc.getArgument().getAsTemplate().getAsTemplateDecl()),
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(),
			getParseLocation(loc.getLocation())
		);
	}
}

void CxxAstVisitorComponentIndexer::beginTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture)
{
	if ((!lambdaExpr->isInitCapture(capture)) && (capture->capturesVariable()))
	{
		clang::VarDecl* d = capture->getCapturedVar();
		SymbolKind symbolKind = getSymbolKind(d);
		if (symbolKind == SYMBOL_LOCAL_VARIABLE || symbolKind == SYMBOL_PARAMETER)
		{
			if (!d->getNameAsString().empty()) // don't record anonymous parameters
			{
				ParseLocation declLocation = getParseLocation(d->getLocation());
				std::string name =
					declLocation.filePath.fileName() + "<" +
					std::to_string(declLocation.startLineNumber) + ":" +
					std::to_string(declLocation.startColumnNumber) + ">";
				m_client->onLocalSymbolParsed(name, getParseLocation(capture->getLocation()));
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitTagDecl(clang::TagDecl* d)
{
	if (shouldVisitDecl(d))
	{
		DefinitionKind definitionKind = DEFINITION_NONE;
		if (d->isThisDeclarationADefinition())
		{
			definitionKind = utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT;
		}

		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			utility::convertTagKind(d->getTagKind()),
			getParseLocation(d->getLocation()),
			getParseLocationOfTagDeclBody(d),
			utility::convertAccessSpecifier(d->getAccess()),
			definitionKind
		);
	}
}

void CxxAstVisitorComponentIndexer::visitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d)
{
	if (shouldVisitDecl(d))
	{
		clang::NamedDecl* specializedFromDecl;

		// todo: use context and childcontext!!
		llvm::PointerUnion<clang::ClassTemplateDecl*, clang::ClassTemplatePartialSpecializationDecl*> pu = d->getSpecializedTemplateOrPartial();
		if (pu.is<clang::ClassTemplateDecl*>())
		{
			specializedFromDecl = pu.get<clang::ClassTemplateDecl*>();
		}
		else if (pu.is<clang::ClassTemplatePartialSpecializationDecl*>())
		{
			specializedFromDecl = pu.get<clang::ClassTemplatePartialSpecializationDecl*>();
		}

		m_client->recordReference(
			REFERENCE_TEMPLATE_SPECIALIZATION,
			getAstVisitor()->getDeclNameCache()->getValue(specializedFromDecl),
			getAstVisitor()->getDeclNameCache()->getValue(d),
			getParseLocation(d->getLocation())
		);
	}
}

void CxxAstVisitorComponentIndexer::visitVarDecl(clang::VarDecl* d)
{
	if (shouldVisitDecl(d))
	{
		SymbolKind symbolKind = getSymbolKind(d);
		if (symbolKind == SYMBOL_LOCAL_VARIABLE || symbolKind == SYMBOL_PARAMETER)
		{
			if (!d->getNameAsString().empty()) // don't record anonymous parameters
			{
				ParseLocation declLocation = getParseLocation(d->getLocation());
				std::string name =
					declLocation.filePath.fileName() + "<" +
					std::to_string(declLocation.startLineNumber) + ":" +
					std::to_string(declLocation.startColumnNumber) + ">";
				m_client->onLocalSymbolParsed(name, getParseLocation(d->getLocation()));
			}
		}
		else
		{
			m_client->recordSymbol(
				getAstVisitor()->getDeclNameCache()->getValue(d),
				symbolKind,
				getParseLocation(d->getLocation()),
				utility::convertAccessSpecifier(d->getAccess()),
				utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
			);
		}
	}
}

void CxxAstVisitorComponentIndexer::visitFieldDecl(clang::FieldDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_FIELD,
			getParseLocation(d->getLocation()),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitFunctionDecl(clang::FunctionDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			clang::isa<clang::CXXMethodDecl>(d) ? SYMBOL_METHOD : SYMBOL_FUNCTION,
			getParseLocation(d->getLocation()),
			getParseLocationOfFunctionBody(d),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);

		if (d->isFunctionTemplateSpecialization())
		{
			const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(d->getPrimaryTemplate()->getTemplatedDecl()); // todo: use context and childcontext!!

			m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);

			m_client->recordReference(
				REFERENCE_TEMPLATE_SPECIALIZATION,
				referencedName,
				getAstVisitor()->getDeclNameCache()->getValue(d),
				getParseLocation(d->getLocation())
			);
		}
	}
}

void CxxAstVisitorComponentIndexer::visitCXXMethodDecl(clang::CXXMethodDecl* d)
{
	// Decl has been recorded in VisitFunctionDecl
	if (shouldVisitDecl(d))
	{
		for (clang::CXXMethodDecl::method_iterator it = d->begin_overridden_methods(); // TODO: iterate in traversal and use REFERENCE_OVERRIDE or so..
			it != d->end_overridden_methods(); it++)
		{
			const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(*it);

			m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);

			m_client->recordReference(
				REFERENCE_OVERRIDE,
				referencedName,
				getAstVisitor()->getDeclNameCache()->getValue(d),
				getParseLocation(d->getLocation())
			);
		}

		clang::MemberSpecializationInfo* memberSpecializationInfo = d->getMemberSpecializationInfo();
		if (memberSpecializationInfo)
		{
			clang::NamedDecl* specializedNamedDecl = memberSpecializationInfo->getInstantiatedFrom();
			if (clang::isa<clang::FunctionDecl>(specializedNamedDecl))
			{
				const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(specializedNamedDecl);

				m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);

				m_client->recordReference(
					REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION,
					referencedName,
					getAstVisitor()->getDeclNameCache()->getValue(d),
					getParseLocation(d->getLocation())
				);
			}
		}
	}
}

void CxxAstVisitorComponentIndexer::visitEnumConstantDecl(clang::EnumConstantDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_ENUM_CONSTANT,
			getParseLocation(d->getLocation()),
			ACCESS_NONE,
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitNamespaceDecl(clang::NamespaceDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_NAMESPACE,
			getParseLocation(d->getLocation()),
			getParseLocation(d->getSourceRange()),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_NAMESPACE,
			getParseLocation(d->getLocation()),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);

		m_client->recordReference(
			REFERENCE_USAGE,
			getAstVisitor()->getDeclNameCache()->getValue(d->getAliasedNamespace()),
			getAstVisitor()->getDeclNameCache()->getValue(d),
			getParseLocation(d->getTargetNameLoc())
		);
	}
}

void CxxAstVisitorComponentIndexer::visitTypedefDecl(clang::TypedefDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_TYPEDEF,
			getParseLocation(d->getLocation()),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitTypeAliasDecl(clang::TypeAliasDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_TYPEDEF,
			getParseLocation(d->getLocation()),
			utility::convertAccessSpecifier(d->getAccess()),
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d)
{
	if (shouldVisitDecl(d))
	{
		const NameHierarchy nameHierarchy = getAstVisitor()->getDeclNameCache()->getValue(d->getNominatedNamespaceAsWritten());

		m_client->recordSymbol(nameHierarchy, SYMBOL_NAMESPACE, ACCESS_NONE, DEFINITION_NONE);

		ParseLocation loc = getParseLocation(d->getLocation());
		m_client->recordReference(
			REFERENCE_USAGE,
			nameHierarchy,
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(NameHierarchy(loc.filePath.str())),
			loc
		);
	}
}

void CxxAstVisitorComponentIndexer::visitUsingDecl(clang::UsingDecl* d)
{
	if (shouldVisitDecl(d))
	{
		ParseLocation loc = getParseLocation(d->getLocation());
		m_client->recordReference(
			REFERENCE_USAGE,
			getAstVisitor()->getDeclNameCache()->getValue(d),
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(NameHierarchy(loc.filePath.str())),
			loc
		);
	}
}

void CxxAstVisitorComponentIndexer::visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			utility::isImplicit(d) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitTypeLoc(clang::TypeLoc tl)
{
	if ((shouldVisitReference(tl.getBeginLoc(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl())) &&
		(!getAstVisitor()->checkIgnoresTypeLoc(tl)))
	{
		if (clang::dyn_cast_or_null<clang::BuiltinType>(tl.getTypePtr()))
		{
			m_client->recordSymbol(getAstVisitor()->getTypeNameCache()->getValue(tl.getTypePtr()), SYMBOL_BUILTIN_TYPE, ACCESS_NONE, DEFINITION_EXPLICIT);
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

		m_client->recordReference(
			getAstVisitor()->getComponent<CxxAstVisitorComponentTypeRefKind>()->getReferenceKind(),
			getAstVisitor()->getTypeNameCache()->getValue(tl.getTypePtr()),
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(1), // we skip the last element because it refers to this typeloc.
			getParseLocation(loc)
		);
	}
}

void CxxAstVisitorComponentIndexer::visitDeclRefExpr(clang::DeclRefExpr* s)
{
	clang::ValueDecl* decl = s->getDecl();
	if (shouldVisitReference(s->getLocation(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	{
		if ((clang::isa<clang::ParmVarDecl>(decl)) ||
			(clang::isa<clang::VarDecl>(decl) && decl->getParentFunctionOrMethod() != NULL)
			) {
			ParseLocation declLocation = getParseLocation(decl->getLocation());
			std::string name = declLocation.filePath.fileName() + "<" +
				std::to_string(declLocation.startLineNumber) + ":" +
				std::to_string(declLocation.startColumnNumber) + ">";

			m_client->onLocalSymbolParsed(name, getParseLocation(s->getLocation()));
		}
		else
		{
			const ReferenceKind refKind = consumeDeclRefContextKind();
			const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(s->getDecl());

			if (refKind == REFERENCE_CALL)
			{
				m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);
			}

			m_client->recordReference(
				refKind,
				referencedName,
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(),
				getParseLocation(s->getLocation())
			);
		}
	}
}

void CxxAstVisitorComponentIndexer::visitMemberExpr(clang::MemberExpr* s)
{
	if (shouldVisitReference(s->getMemberLoc(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	{
		const ReferenceKind refKind = consumeDeclRefContextKind();
		const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(s->getMemberDecl());

		if (refKind == REFERENCE_CALL)
		{
			m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);
		}

		m_client->recordReference(
			refKind,
			referencedName,
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(),
			getParseLocation(s->getMemberLoc())
		);
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

	if (shouldVisitReference(s->getLocation(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	{
		//if (e->getParenOrBraceRange().isValid()) {
		//    // XXX: This code is a kludge.  Recording calls to constructors is
		//    // troublesome because there isn't an obvious location to associate the
		//    // call with.  Consider:
		//    //     A::A() : field(1, 2, 3) {}
		//    //     new A<B>(1, 2, 3)
		//    //     struct A { A(B); }; A f() { B b; return b; }
		//    // Implicit calls to conversion operator methods pose a similar
		//    // problem.
		//    //
		//    // Recording constructor calls is very useful, though, so, as a
		//    // temporary measure, when there are constructor arguments surrounded
		//    // by parentheses, associate the call with the right parenthesis.
		//    //
		//    // Perhaps the right fix is to associate the call with the line itself
		//    // or with a larger span which may have other references nested within
		//    // it.  The fix may have implications for the navigator GUI.
		//    RecordDeclRefExpr(
		//                e->getConstructor(),
		//                e->getParenOrBraceRange().getEnd(),
		//                e,
		//                CF_Called);
		//}
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
		loc = clang::Lexer::GetBeginningOfToken(loc, m_astContext->getSourceManager(), m_astContext->getLangOpts());

		const ReferenceKind refKind = consumeDeclRefContextKind();
		const NameHierarchy referencedName = getAstVisitor()->getDeclNameCache()->getValue(s->getConstructor());

		if (refKind == REFERENCE_CALL)
		{
			m_client->recordSymbol(referencedName, SYMBOL_FUNCTION, ACCESS_NONE, DEFINITION_NONE);
		}

		m_client->recordReference(
			refKind,
			referencedName,
			getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(),
			getParseLocation(loc)
		);
	}
}

void CxxAstVisitorComponentIndexer::visitLambdaExpr(clang::LambdaExpr* s)
{
	clang::CXXMethodDecl* methodDecl = s->getCallOperator();
	if (shouldVisitDecl(methodDecl))
	{
		m_client->recordSymbol(
			getAstVisitor()->getDeclNameCache()->getValue(methodDecl),
			SYMBOL_FUNCTION,
			getParseLocation(s->getLocStart()),
			getParseLocationOfFunctionBody(methodDecl),
			ACCESS_NONE,  // TODO: introduce AccessLambda
			utility::isImplicit(methodDecl) ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT
		);
	}
}

void CxxAstVisitorComponentIndexer::visitConstructorInitializer(clang::CXXCtorInitializer* init)
{
	if (shouldVisitReference(init->getMemberLocation(), getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getTopmostContextDecl()))
	{
		// record the field usage here because it is not a DeclRefExpr
		if (clang::FieldDecl* memberDecl = init->getMember())
		{
			m_client->recordReference(
				REFERENCE_USAGE,
				getAstVisitor()->getDeclNameCache()->getValue(memberDecl),
				getAstVisitor()->getComponent<CxxAstVisitorComponentContext>()->getContextName(),
				getParseLocation(init->getMemberLocation())
			);
		}
	}
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocationOfTagDeclBody(clang::TagDecl* decl) const
{
	return getAstVisitor()->getParseLocationOfTagDeclBody(decl);
}

ParseLocation CxxAstVisitorComponentIndexer::getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const
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

ReferenceKind CxxAstVisitorComponentIndexer::consumeDeclRefContextKind()
{
	ReferenceKind refKind = REFERENCE_UNDEFINED;

	std::shared_ptr<CxxAstVisitorComponentTypeRefKind> typeRefKindComponent = getAstVisitor()->getComponent<CxxAstVisitorComponentTypeRefKind>();

	if (typeRefKindComponent->getReferenceKind() == REFERENCE_TYPE_USAGE)
	{
		refKind = getAstVisitor()->getComponent<CxxAstVisitorComponentDeclRefKind>()->getReferenceKind();
	}
	else
	{
		refKind = typeRefKindComponent->getReferenceKind();
	}
	return refKind;
}

SymbolKind CxxAstVisitorComponentIndexer::getSymbolKind(clang::VarDecl* d)
{
	SymbolKind symbolKind = SYMBOL_KIND_MAX;

	if (llvm::isa<clang::ParmVarDecl>(d))
	{
		symbolKind = SYMBOL_PARAMETER;
	}
	else if (d->getParentFunctionOrMethod() == NULL)
	{
		if (d->getAccess() == clang::AS_none)
		{
			symbolKind = SYMBOL_GLOBAL_VARIABLE;
		}
		else
		{
			symbolKind = SYMBOL_FIELD;
		}
	}
	else
	{
		symbolKind = SYMBOL_LOCAL_VARIABLE;
	}

	return symbolKind;
}

bool CxxAstVisitorComponentIndexer::shouldVisitDecl(const clang::Decl* decl)
{
	if (decl)
	{
		clang::SourceLocation loc = decl->getLocation();
		bool declIsImplicit = utility::isImplicit(decl);
		if ((declIsImplicit && isLocatedInProjectFile(loc)) ||
			(!declIsImplicit && isLocatedInUnparsedProjectFile(loc)))
		{
			return true;
		}
	}
	return false;
}

bool CxxAstVisitorComponentIndexer::shouldVisitReference(const clang::SourceLocation& referenceLocation, const clang::Decl* contextDecl)
{
	bool declIsImplicit = true; // default value is "true" to make sure that everything that should be visited gets visited.
	if (contextDecl)
	{
		declIsImplicit = utility::isImplicit(contextDecl);
	}

	if ((declIsImplicit && isLocatedInProjectFile(referenceLocation)) ||
		(!declIsImplicit && isLocatedInUnparsedProjectFile(referenceLocation)))
	{
		return true;
	}
	return false;
}

bool CxxAstVisitorComponentIndexer::isLocatedInUnparsedProjectFile(clang::SourceLocation loc)
{
	clang::SourceManager& sourceManager = m_astContext->getSourceManager();
	clang::SourceLocation spellingLoc = sourceManager.getSpellingLoc(loc);

	clang::FileID fileId;

	if (spellingLoc.isValid())
	{
		if (sourceManager.isWrittenInMainFile(spellingLoc))
		{
			return true;
		}

		fileId = sourceManager.getFileID(spellingLoc);
	}

	if (fileId.isValid())
	{
		auto it = m_inUnparsedProjectFileMap.find(fileId);
		if (it != m_inUnparsedProjectFileMap.end())
		{
			return it->second;
		}

		bool ret = false;
		const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
		if (fileEntry != NULL)
		{
			FilePath filePath = getAstVisitor()->getCanonicalFilePathCache()->getValue(fileEntry->getName());

			if (m_fileRegister->hasFilePath(filePath))
			{
				ret = !(m_fileRegister->fileIsIndexed(filePath));
			}
		}

		m_inUnparsedProjectFileMap[fileId] = ret;
		return ret;
	}

	return false;
}

bool CxxAstVisitorComponentIndexer::isLocatedInProjectFile(clang::SourceLocation loc)
{
	clang::SourceManager& sourceManager = m_astContext->getSourceManager();
	clang::SourceLocation spellingLoc = sourceManager.getSpellingLoc(loc);

	clang::FileID fileId;

	if (spellingLoc.isValid())
	{
		fileId = sourceManager.getFileID(spellingLoc);
	}

	if (!fileId.isInvalid())
	{
		auto it = m_inProjectFileMap.find(fileId);
		if (it != m_inProjectFileMap.end())
		{
			return it->second;
		}

		const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
		if (fileEntry != NULL)
		{
			std::string fileName = fileEntry->getName();
			FilePath filePath = getAstVisitor()->getCanonicalFilePathCache()->getValue(fileName);
			bool ret = m_fileRegister->hasFilePath(filePath);
			m_inProjectFileMap[fileId] = ret;
			return ret;
		}
	}

	return false;
}
