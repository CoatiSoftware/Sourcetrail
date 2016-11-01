#include "data/parser/cxx/CxxAstVisitor.h"

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Preprocessor.h>

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"
#include "utility/ScopedFunctor.h"
#include "utility/ScopedSwitcher.h"

#include "data/parser/cxx/name_resolver/CxxDeclNameResolver.h"
#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"

CxxAstVisitor::CxxAstVisitor(clang::ASTContext* astContext, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
	: m_astContext(astContext)
	, m_preprocessor(preprocessor)
	, m_client(client)
	, m_fileRegister(fileRegister)
	, m_typeRefContext(REFERENCE_TYPE_USAGE)
	, m_declRefContext(REFERENCE_USAGE)
{
	m_declNameCache = std::make_shared<DeclNameCache>([](const clang::NamedDecl* decl) -> NameHierarchy
	{
		if (decl)
		{
			CxxDeclNameResolver resolver(decl);
			return resolver.getDeclNameHierarchy();
		}
		return NameHierarchy("global");
	});
	m_typeNameCache = std::make_shared<TypeNameCache>([](const clang::Type* type) -> NameHierarchy
	{
		if (type)
		{
			CxxTypeNameResolver resolver;
			return resolver.getTypeNameHierarchy(type);
		}
		return NameHierarchy("global");
	});
}

CxxAstVisitor::~CxxAstVisitor()
{
}

void CxxAstVisitor::indexDecl(clang::Decl* d)
{
	this->TraverseDecl(d);
}

bool CxxAstVisitor::shouldVisitTemplateInstantiations() const
{
	return true;
}

bool CxxAstVisitor::shouldVisitImplicitCode() const
{
	return true;
}

bool CxxAstVisitor::TraverseDecl(clang::Decl* d)
{
	std::shared_ptr<ScopedFunctor> removeContextFunctor;
	if (d &&
		clang::isa<clang::NamedDecl>(d) &&
		!clang::isa<clang::ParmVarDecl>(d) &&											// no parameter
		!(clang::isa<clang::VarDecl>(d) && d->getParentFunctionOrMethod() != NULL) &&	// no local variable
		!clang::isa<clang::UsingDirectiveDecl>(d) &&									// no using directive decl
		//!clang::isa<clang::UsingDecl>(d) &&												// no using decl
		!clang::isa<clang::NamespaceDecl>(d)											// no namespace
	){
		clang::NamedDecl* nd = clang::dyn_cast<clang::NamedDecl>(d);
		m_contextStack.push_back(std::make_shared<CxxContextDecl>(nd, m_declNameCache));
		removeContextFunctor = std::make_shared<ScopedFunctor>([this](){ m_contextStack.pop_back(); });
	}
	return base::TraverseDecl(d);
}

bool CxxAstVisitor::TraverseStmt(clang::Stmt* stmt)
{
	return base::TraverseStmt(stmt);
}

bool CxxAstVisitor::TraverseType(clang::QualType t)
{
	return base::TraverseType(t);
}

bool CxxAstVisitor::TraverseTypeLoc(clang::TypeLoc tl)
{
	std::shared_ptr<ScopedFunctor> removeContextFunctor;
	if (!checkIgnoresTypeLoc(tl))
	{
		m_contextStack.push_back(std::make_shared<CxxContextType>(tl.getTypePtr(), m_typeNameCache));
		removeContextFunctor = std::make_shared<ScopedFunctor>([this](){ m_contextStack.pop_back(); });
	}
	return base::TraverseTypeLoc(tl);
}

// same as base::TraverseCXXRecordDecl(..) but we need to integrate the setter for the context info.
// additionally: skip implicit CXXRecordDecls (this does not skip template specializations).
bool CxxAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl *d)
{
	if (isImplicit(d))
	{
		return true;
	}

	WalkUpFromCXXRecordDecl(d);

	TraverseNestedNameSpecifierLoc(d->getQualifierLoc());

	if (d->isCompleteDefinition())
	{
		ScopedSwitcher<ReferenceKind> switcher(m_typeRefContext, REFERENCE_INHERITANCE);
		for (const auto& base : d->bases())
		{
			if (!TraverseTypeLoc(base.getTypeSourceInfo()->getTypeLoc()))
			{
				return false;
			}
		}
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

// same as base::TraverseTemplateTypeParmDecl(..) but we need to integrate the setter for the context info.
bool CxxAstVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	WalkUpFromTemplateTypeParmDecl(d);

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		ScopedSwitcher<ReferenceKind> switcher(m_typeRefContext, REFERENCE_TEMPLATE_DEFAULT_ARGUMENT);
		TraverseTypeLoc(d->getDefaultArgumentInfo()->getTypeLoc());
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

// same as base::TraverseTemplateTemplateParmDecl(..) but we need to integrate the setter for the context info.
bool CxxAstVisitor::TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	WalkUpFromTemplateTemplateParmDecl(d);

	TraverseDecl(d->getTemplatedDecl());

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		ScopedSwitcher<ReferenceKind> switcher(m_typeRefContext, REFERENCE_TEMPLATE_DEFAULT_ARGUMENT);
		TraverseTemplateArgumentLoc(d->getDefaultArgument());
	}

	clang::TemplateParameterList* TPL = d->getTemplateParameters();
	if (TPL)
	{
		for (clang::TemplateParameterList::iterator I = TPL->begin(), E = TPL->end(); I != E; ++I)
		{
			TraverseDecl(*I);
		}
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

bool CxxAstVisitor::TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc loc)
{
	// just skip all qualifiers (for now)
	return true;
}

bool CxxAstVisitor::TraverseConstructorInitializer(clang::CXXCtorInitializer* init)
{
	if (!VisitConstructorInitializer(init))
	{
		return false;
	}
	return base::TraverseConstructorInitializer(init);
}

bool CxxAstVisitor::TraverseCallExpr(clang::CallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* s)
{
	return TraverseCallCommon(s);
}

bool CxxAstVisitor::TraverseCXXConstructExpr(clang::CXXConstructExpr* s)
{
	{
		ScopedSwitcher<ReferenceKind> switcher(m_declRefContext, REFERENCE_CALL);
		WalkUpFromCXXConstructExpr(s);
	}
	for (unsigned int i = 0; i < s->getNumArgs(); ++i) {
		clang::Expr *arg = s->getArg(i);
		TraverseStmt(arg);
	}
	return true;
}

bool CxxAstVisitor::TraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s)
{
	ScopedSwitcher<ReferenceKind> switcher(m_declRefContext, REFERENCE_CALL);
	return base::TraverseCXXTemporaryObjectExpr(s);
}

bool CxxAstVisitor::TraverseLambdaExpr(clang::LambdaExpr* s)
{
	clang::CXXMethodDecl* methodDecl = s->getCallOperator();
	m_contextStack.push_back(std::make_shared<CxxContextDecl>(methodDecl, m_declNameCache));
	std::shared_ptr<ScopedFunctor> removeContextFunctor = std::make_shared<ScopedFunctor>([this](){ m_contextStack.pop_back(); });
	return base::TraverseLambdaExpr(s);
}

bool CxxAstVisitor::TraverseFunctionDecl(clang::FunctionDecl* d)
{
	ScopedSwitcher<std::shared_ptr<CxxContext>> switcher(
		m_templateArgumentContext, std::make_shared<CxxContextDecl>(d, m_declNameCache)
	);
	return base::TraverseFunctionDecl(d);
}

bool CxxAstVisitor::TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d)
{
	ScopedSwitcher<std::shared_ptr<CxxContext>> switcher(
		m_templateArgumentContext, std::make_shared<CxxContextDecl>(d, m_declNameCache)
	);
	return base::TraverseClassTemplateSpecializationDecl(d);
}

bool CxxAstVisitor::TraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	ScopedSwitcher<std::shared_ptr<CxxContext>> switcher(
		m_templateArgumentContext, std::make_shared<CxxContextDecl>(d, m_declNameCache)
	);
	return base::TraverseClassTemplatePartialSpecializationDecl(d);
}

bool CxxAstVisitor::TraverseDeclRefExpr(clang::DeclRefExpr* s)
{
	ScopedSwitcher<std::shared_ptr<CxxContext>> switcher(
		m_templateArgumentContext, std::make_shared<CxxContextDecl>(s->getDecl(), m_declNameCache)
	);
	return base::TraverseDeclRefExpr(s);
}

bool CxxAstVisitor::TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc)
{
	const clang::Type* t = loc.getTypePtr();
	ScopedSwitcher<std::shared_ptr<CxxContext>> switcher(
		m_templateArgumentContext, std::make_shared<CxxContextType>(t, m_typeNameCache)
	);
	return base::TraverseTemplateSpecializationTypeLoc(loc);
}

bool CxxAstVisitor::TraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) // TODO: do this for unresolved and dependent stuff
{
	std::shared_ptr<CxxContext> clear;
	ScopedSwitcher<std::shared_ptr<CxxContext>> sw(m_templateArgumentContext, clear);
	return base::TraverseUnresolvedLookupExpr(e);
}


bool CxxAstVisitor::TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	std::shared_ptr<ScopedSwitcher<ReferenceKind>> switcher;
	std::shared_ptr<ScopedFunctor> removeContextFunctor;

	if (m_typeRefContext != REFERENCE_TEMPLATE_DEFAULT_ARGUMENT && m_templateArgumentContext)
	{
		switcher = std::make_shared<ScopedSwitcher<ReferenceKind>>(m_typeRefContext, REFERENCE_TEMPLATE_ARGUMENT);

		m_contextStack.push_back(m_templateArgumentContext);
		removeContextFunctor = std::make_shared<ScopedFunctor>([this](){ m_contextStack.pop_back(); });
	}

	if (loc.getArgument().getKind() == clang::TemplateArgument::Template)
	{
		// TODO: maybe move this to VisitTemplateName
		m_client->recordReference(
			m_typeRefContext,
			m_declNameCache->getValue(loc.getArgument().getAsTemplate().getAsTemplateDecl()),
			getContextName(),
			getParseLocation(loc.getLocation())
		);
	}

	return base::TraverseTemplateArgumentLoc(loc);
}

void CxxAstVisitor::traverseDeclContextHelper(clang::DeclContext *d)
{
	if (!d)
		return;

	// Traverse children.
	for (clang::DeclContext::decl_iterator it = d->decls_begin(),
		itEnd = d->decls_end(); it != itEnd; ++it) {
		// BlockDecls are traversed through BlockExprs.
		if (!llvm::isa<clang::BlockDecl>(*it))
			TraverseDecl(*it);
	}
}

bool CxxAstVisitor::TraverseCallCommon(clang::CallExpr* s)
{
	{
		ScopedSwitcher<ReferenceKind> switcher(m_declRefContext, REFERENCE_CALL);
		TraverseStmt(s->getCallee());
	}
	for (unsigned int i = 0; i < s->getNumArgs(); ++i) {
		clang::Expr *arg = s->getArg(i);
		TraverseStmt(arg);
	}
	return true;
}

bool CxxAstVisitor::VisitTagDecl(clang::TagDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			convertTagKind(d->getTagKind()),
			getParseLocation(d->getLocation()),
			getParseLocationOfTagDeclBody(d),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d)
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
			REFERENCE_TEMPLATE_SPECIALIZATION_OF, // TODO: call this REFERENCE_TEMPLATE_SPECIALIZATION and reverse the following arguments
			m_declNameCache->getValue(specializedFromDecl),
			m_declNameCache->getValue(d),
			getParseLocation(d->getLocation())
		);
	}
	return true;
}

bool CxxAstVisitor::VisitFunctionDecl(clang::FunctionDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			clang::isa<clang::CXXMethodDecl>(d) ? SYMBOL_METHOD : SYMBOL_FUNCTION,
			getParseLocation(d->getLocation()),
			getParseLocationOfFunctionBody(d),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);

		if (d->isFunctionTemplateSpecialization())
		{
			m_client->recordReference(
				REFERENCE_TEMPLATE_SPECIALIZATION_OF, // TODO: call this REFERENCE_TEMPLATE_SPECIALIZATION and reverse the following arguments
				m_declNameCache->getValue(d->getPrimaryTemplate()->getTemplatedDecl()), // todo: use context and childcontext!!
				m_declNameCache->getValue(d),
				getParseLocation(d->getLocation())
			);
		}
	}
	return true;
}

bool CxxAstVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl* d)
{
	// Decl has been recorded in VisitFunctionDecl
	if (shouldVisitDecl(d))
	{
		for (clang::CXXMethodDecl::method_iterator it = d->begin_overridden_methods(); // iterate in traversal and use RT_Overridden or so..
			it != d->end_overridden_methods(); it++)
		{
			m_client->recordReference(
				REFERENCE_OVERRIDE,
				m_declNameCache->getValue(*it),
				m_declNameCache->getValue(d),
				getParseLocation(d->getLocation())
			);
		}

		clang::MemberSpecializationInfo* memberSpecializationInfo = d->getMemberSpecializationInfo();
		if (memberSpecializationInfo)
		{
			clang::NamedDecl* specializedNamedDecl = memberSpecializationInfo->getInstantiatedFrom();
			if (clang::isa<clang::FunctionDecl>(specializedNamedDecl))
			{
				m_client->recordReference(
					REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION_OF,
					m_declNameCache->getValue(specializedNamedDecl),
					m_declNameCache->getValue(d),
					getParseLocation(d->getLocation())
				);
			}
		}
	}
	return true;
}

bool CxxAstVisitor::VisitVarDecl(clang::VarDecl* d)
{
	if (shouldVisitDecl(d))
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
				m_declNameCache->getValue(d),
				symbolKind,
				getParseLocation(d->getLocation()),
				convertAccessSpecifier(d->getAccess()),
				isImplicit(d)
			);
		}
	}
	return true;
}

bool CxxAstVisitor::VisitFieldDecl(clang::FieldDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_FIELD,
			getParseLocation(d->getLocation()),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitTypedefDecl(clang::TypedefDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_TYPEDEF,
			getParseLocation(d->getLocation()),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitTypeAliasDecl(clang::TypeAliasDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_TYPEDEF,
			getParseLocation(d->getLocation()),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitNamespaceDecl(clang::NamespaceDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_NAMESPACE,
			d->isAnonymousNamespace() ? ParseLocation() : getParseLocation(d->getLocation()),
			getParseLocation(d->getSourceRange()),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitNamespaceAliasDecl(clang::NamespaceAliasDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_NAMESPACE,
			getParseLocation(d->getLocation()),
			convertAccessSpecifier(d->getAccess()),
			isImplicit(d)
		);

		m_client->recordReference(
			REFERENCE_USAGE,
			m_declNameCache->getValue(d->getAliasedNamespace()),
			m_declNameCache->getValue(d),
			getParseLocation(d->getTargetNameLoc())
		);
	}
	return true;
}

bool CxxAstVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_ENUM_CONSTANT,
			getParseLocation(d->getLocation()),
			ACCESS_NONE,
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitUsingDirectiveDecl(clang::UsingDirectiveDecl* d)
{
	if (shouldVisitDecl(d))
	{
		m_client->recordReference(
			REFERENCE_USAGE,
			m_declNameCache->getValue(d->getNominatedNamespaceAsWritten()),
			getContextName(),			// TODO: use file here, if no context available
			getParseLocation(d->getLocation())
		);
	}
	return true;
}

bool CxxAstVisitor::VisitUsingDecl(clang::UsingDecl* d)
{
	//if (shouldVisitDecl(d))
	//{
	//m_client->recordReference(
	//	REFERENCE_USAGE,
	//	m_declNameCache->getValue(d),
	//	getContextName(),			// TODO: use file here, if no context available
	//	getParseLocation(d->getLocation())
	//);
	//}
	return true;
}

bool CxxAstVisitor::VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			isImplicit(d)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	if (shouldVisitDecl(d) && !d->getName().empty()) // We don't create symbols for unnamed template parameters.
	{
		m_client->recordSymbol(
			m_declNameCache->getValue(d),
			SYMBOL_TEMPLATE_PARAMETER,
			getParseLocation(d->getLocation()),
			ACCESS_TEMPLATE_PARAMETER,
			isImplicit(d)
		);
	}
	return true;
}



/*
bool CxxAstVisitor::VisitNamedDecl(clang::NamedDecl* d)
{
	if (!shouldVisitDecl(d))
	{
		return true;
	}




	//if (clang::UsingDirectiveDecl *ud = llvm::dyn_cast<clang::UsingDirectiveDecl>(d))
	//{
	//	m_client->recordReference(
	//		REFERENCE_USAGE,
	//		m_declNameCache->getValue(ud->getNominatedNamespaceAsWritten()),
	//		m_con

	//	return true;


	//	RecordDeclRef(
	//		ud->getNominatedNamespaceAsWritten(),
	//		loc, RT_UsingDirective);
	//}
	//else if (clang::UsingDecl *usd = llvm::dyn_cast<clang::UsingDecl>(d))
	//{
	//	for (auto it = usd->shadow_begin(), itEnd = usd->shadow_end();
	//		it != itEnd; ++it) {
	//		clang::UsingShadowDecl *shadow = *it;
	//		RecordDeclRef(shadow->getTargetDecl(), loc, RT_Using);
	//	}
	//} else


	if (llvm::isa<clang::FunctionTemplateDecl>(d)) {
		// TODO: use these cases for creating the connection between two undefined template things
		// Do nothing.  The function will be recorded when it appears as a
		// FunctionDecl.
	} else if (llvm::isa<clang::ClassTemplateDecl>(d)) {
		// Do nothing.  The class will be recorded when it appears as a
		// RecordDecl.
	} else if (llvm::isa<clang::TypeAliasTemplateDecl>(d)) {
		// Do nothing.  The type alias will be recorded when it appears as a
		// TypeAliasDecl.
	}

	return true;
}

*/







bool CxxAstVisitor::VisitTypeLoc(clang::TypeLoc tl)
{
	if ((isLocatedInUnparsedProjectFile(tl.getBeginLoc())) &&
		(!checkIgnoresTypeLoc(tl)))
	{
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
			m_typeRefContext,
			m_typeNameCache->getValue(tl.getTypePtr()),
			getContextName(1), // we skip the last element because it refers to this typeloc.
			getParseLocation(loc)
		);
	}
	return true;
}

bool CxxAstVisitor::VisitDeclRefExpr(clang::DeclRefExpr* s)
{
	clang::ValueDecl* decl = s->getDecl();

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
		m_client->recordReference(
			m_typeRefContext == REFERENCE_TYPE_USAGE ? m_declRefContext : m_typeRefContext,
			m_declNameCache->getValue(s->getDecl()),
			getContextName(),
			getParseLocation(s->getLocation())
		);
	}

	return true;
}

bool CxxAstVisitor::VisitMemberExpr(clang::MemberExpr* s)
{
	m_client->recordReference(
		m_typeRefContext == REFERENCE_TYPE_USAGE ? m_declRefContext : m_typeRefContext,
		m_declNameCache->getValue(s->getMemberDecl()),
		getContextName(),
		getParseLocation(s->getMemberLoc())
	);
	return true;
}

bool CxxAstVisitor::VisitCXXConstructExpr(clang::CXXConstructExpr* s)
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

	m_client->recordReference(
		m_typeRefContext == REFERENCE_TYPE_USAGE ? m_declRefContext : m_typeRefContext,
		m_declNameCache->getValue(s->getConstructor()),
		getContextName(),
		getParseLocation(loc)
	);
	return true;
}

bool CxxAstVisitor::VisitLambdaExpr(clang::LambdaExpr* s)
{
	clang::CXXMethodDecl* methodDecl = s->getCallOperator();
	m_client->recordSymbol(
		m_declNameCache->getValue(methodDecl),
		SYMBOL_FUNCTION,
		getParseLocation(s->getLocStart()),
		getParseLocationOfFunctionBody(methodDecl),
		ACCESS_NONE,  // TODO: introduce AccessLambda
		isImplicit(methodDecl)
	);
	return true;
}

bool CxxAstVisitor::VisitConstructorInitializer(clang::CXXCtorInitializer* init)
{
	// record the field usage here because it is not a DeclRefExpr
	if (clang::FieldDecl* memberDecl = init->getMember())
	{
		m_client->recordReference(
			REFERENCE_USAGE,
			m_declNameCache->getValue(memberDecl),
			getContextName(),
			getParseLocation(init->getMemberLocation())
		);
	}
	return true;
}

bool CxxAstVisitor::isImplicit(clang::Decl* d) const
{
	if (!d)
	{
		return false;
	}

	if (d->isImplicit())
	{
		if (clang::RecordDecl* rd = clang::dyn_cast_or_null<clang::RecordDecl>(d))
		{
			if (rd->isLambda())
			{
				return isImplicit(clang::dyn_cast_or_null<clang::Decl>(d->getDeclContext()));
			}
		}
		return true;
	}
	else if (clang::ClassTemplateSpecializationDecl* ctsd = clang::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(d))
	{
		if (!ctsd->isExplicitSpecialization())
		{
			return true;
		}
	}
	else if (clang::FunctionDecl* fd = clang::dyn_cast_or_null<clang::FunctionDecl>(d))
	{
		if (fd->isTemplateInstantiation() && fd->getTemplateSpecializationKind() != clang::TSK_ExplicitSpecialization) // or undefined??
		{
			return true;
		}
	}

	return isImplicit(clang::dyn_cast_or_null<clang::Decl>(d->getDeclContext()));
}

bool CxxAstVisitor::shouldVisitDecl(clang::Decl* d)
{
	clang::SourceLocation loc = d->getLocation();

	bool declIsImplicit = isImplicit(d);

	if (!d ||
		(declIsImplicit && !isLocatedInProjectFile(loc)) ||
		(!declIsImplicit && !isLocatedInUnparsedProjectFile(loc)))
	{
		return false;
	}

	return true;
}

bool CxxAstVisitor::isLocatedInUnparsedProjectFile(clang::SourceLocation loc)
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
		auto it = m_inUnparsedProjectFileMap.find(fileId);
		if (it != m_inUnparsedProjectFileMap.end())
		{
			return it->second;
		}

		bool ret = false;
		if (sourceManager.isWrittenInMainFile(spellingLoc))
		{
			ret = true;
		}
		else
		{
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
			if (fileEntry != NULL)
			{
				std::string fileName = fileEntry->getName();
				FilePath filePath = FilePath(fileName).canonical();

				if (m_fileRegister->hasIncludeFile(filePath))
				{
					ret = !(m_fileRegister->includeFileIsParsed(filePath));
				}
			}
		}
		m_inUnparsedProjectFileMap[fileId] = ret;
		return ret;
	}
	return false;
}

bool CxxAstVisitor::isLocatedInProjectFile(clang::SourceLocation loc)
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
			FilePath filePath = FilePath(fileName).canonical();
			bool ret = m_fileRegister->hasFilePath(filePath.str());
			m_inProjectFileMap[fileId] = ret;
			return ret;
		}
	}

	return false;
}

ParseLocation CxxAstVisitor::getParseLocationOfTagDeclBody(clang::TagDecl* decl) const
{
	if (decl->isThisDeclarationADefinition())
	{
		clang::SourceRange range;
		if (clang::CXXRecordDecl* cxxDecl = clang::dyn_cast_or_null<clang::CXXRecordDecl>(decl))
		{
			clang::ClassTemplateDecl* templateDecl = cxxDecl->getDescribedClassTemplate();
			if (templateDecl)
			{
				range = templateDecl->getSourceRange();
			}
		}
		if (range.isInvalid())
		{
			range = decl->getDefinition()->getSourceRange();
		}
		return getParseLocation(range);
	}
	return ParseLocation();
}

ParseLocation CxxAstVisitor::getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const
{
	if (decl->hasBody() && decl->isThisDeclarationADefinition())
	{
		clang::SourceRange range;
		clang::FunctionTemplateDecl* templateDecl = decl->getDescribedFunctionTemplate();
		if (templateDecl)
		{
			range = templateDecl->getSourceRange();
		}
		else
		{
			range = decl->getSourceRange();
		}
		return getParseLocation(range);
	}
	return ParseLocation();
}

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceLocation& loc) const
{
	ParseLocation parseLocation;
	if (loc.isValid())
	{
		clang::SourceManager& sourceManager = m_astContext->getSourceManager();
		clang::SourceLocation startLoc = sourceManager.getSpellingLoc(loc);
		clang::FileID fileId = sourceManager.getFileID(startLoc);

		// find the location file
		if (!fileId.isInvalid())
		{
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
			if (fileEntry != NULL)
			{
				parseLocation.filePath = FilePath(fileEntry->getName()).canonical();
			}
		}

		// find the start location
		{
			unsigned int offset = sourceManager.getFileOffset(startLoc);
			parseLocation.startLineNumber = sourceManager.getLineNumber(fileId, offset);
			parseLocation.startColumnNumber = sourceManager.getColumnNumber(fileId, offset);
		}

		// General case -- find the end of the token starting at loc.
		{
			clang::SourceLocation endSloc = m_preprocessor->getLocForEndOfToken(startLoc);
			unsigned int offset = sourceManager.getFileOffset(endSloc);
			parseLocation.endLineNumber = sourceManager.getLineNumber(fileId, offset);
			parseLocation.endColumnNumber = sourceManager.getColumnNumber(fileId, offset) - 1;
		}
	}


	return parseLocation;
}

ParseLocation CxxAstVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	ParseLocation parseLocation;
	if (sourceRange.isValid())
	{
		const clang::SourceManager& sourceManager = m_astContext->getSourceManager();

		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
		const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

		parseLocation = ParseLocation(
			presumedBegin.getFilename(),
			presumedBegin.getLine(),
			presumedBegin.getColumn(),
			presumedEnd.getLine(),
			presumedEnd.getColumn()
		);
	}
	return parseLocation;
}

AccessKind CxxAstVisitor::convertAccessSpecifier(clang::AccessSpecifier access) const
{
	switch (access)
	{
	case clang::AS_public:
		return ACCESS_PUBLIC;
	case clang::AS_protected:
		return ACCESS_PROTECTED;
	case clang::AS_private:
		return ACCESS_PRIVATE;
	}
	return ACCESS_NONE;
}

SymbolKind CxxAstVisitor::convertTagKind(clang::TagTypeKind tagKind)
{
	switch (tagKind)
	{
	case clang::TTK_Struct:
		return SYMBOL_STRUCT;
	case clang::TTK_Union:
		return SYMBOL_UNION;
	case clang::TTK_Class:
		return SYMBOL_CLASS;
	case clang::TTK_Enum:
		return SYMBOL_ENUM;
	}
	return SYMBOL_KIND_MAX;
}

NameHierarchy CxxAstVisitor::getContextName(const int skip) const
{
	if (m_contextStack.size() <= skip)
	{
		return m_declNameCache->getValue(nullptr);
	}
	return m_contextStack[m_contextStack.size() - 1 - skip]->getName(); // todo: performance optimize this
}

bool CxxAstVisitor::checkIgnoresTypeLoc(const clang::TypeLoc& tl)
{

	if ((!tl.getAs<clang::TagTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TypedefTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull()) ||
		(!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull()) ||
		(!tl.getAs<clang::DependentNameTypeLoc>().isNull()) ||
		(!tl.getAs<clang::BuiltinTypeLoc>().isNull())
	){
		return false;
	}
	return true;
}
