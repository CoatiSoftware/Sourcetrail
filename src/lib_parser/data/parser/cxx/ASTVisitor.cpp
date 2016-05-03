#include "data/parser/cxx/ASTVisitor.h"

#include <clang/AST/Type.h>
#include <clang/Lex/Preprocessor.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/Casting.h>
#include <string>

#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
#include "data/parser/cxx/utilityCxx.h"
#include "data/parser/ParseLocation.h"

#include "utility/file/FileManager.h"
#include "utility/ScopedSwitcher.h"

// TODO: For an array access, X[I], skip over the array-to-pointer decay.  We
// currently record that X's address is taken, which is technically true, but
// the address does not generally escape.

// TODO: A struct assignment in C++ turns into an operator= call, even if the
// user did not define an operator= function.  Consider handling operator=
// calls the same way that normal assignment operations are handled.  Consider
// doing the same for the other overloadable operators.

// TODO: Unary ++ and --
// TODO: Fix local extern variables.  e.g. void func() { extern int var; }
//     Consider extern "C", functions nested within classes or namespaces.

///////////////////////////////////////////////////////////////////////////////
// Misc routines

ASTVisitor::ASTVisitor(clang::ASTContext* context, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
	: m_context(context)
	, m_preprocessor(preprocessor)
	, m_client(client)
	, m_fileRegister(fileRegister)
    , m_thisContext(0)
    , m_childContext(0)
    , m_typeContext(RT_Reference)
	, m_contextAccess(ParserClient::ACCESS_NONE)
{
	m_declNameCache = std::make_shared<DeclNameCache>([](const clang::NamedDecl* decl) -> NameHierarchy
	{
		if (decl)
		{
			return utility::getDeclNameHierarchy(decl);
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

ASTVisitor::~ASTVisitor()
{
}

bool ASTVisitor::VisitTranslationUnitDecl(clang::TranslationUnitDecl* decl)
{
	// decl->dump();
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Dispatcher routines

// Set the "this" context to the "child" context and reset the child context
// to default.

bool ASTVisitor::TraverseStmt(clang::Stmt *stmt)
{
    if (stmt == NULL)
        return true;

    ScopedSwitcher<Context> sw1(m_thisContext, m_childContext);

    if (clang::Expr *e = llvm::dyn_cast<clang::Expr>(stmt)) {
        if (e->isRValue())
            m_thisContext &= ~(CF_AddressTaken | CF_Assigned | CF_Modified);
    } else {
        m_thisContext = 0;
    }

    ScopedSwitcher<Context> sw2(m_childContext, m_thisContext);

    return base::TraverseStmt(stmt);
}

bool ASTVisitor::TraverseType(clang::QualType t)
{
    ScopedSwitcher<Context> sw1(m_thisContext, 0);
    ScopedSwitcher<Context> sw2(m_childContext, 0);
    return base::TraverseType(t);
}

bool ASTVisitor::TraverseTypeLoc(clang::TypeLoc tl)
{
    ScopedSwitcher<Context> sw1(m_thisContext, 0);
    ScopedSwitcher<Context> sw2(m_childContext, 0);
    return base::TraverseTypeLoc(tl);
}

bool ASTVisitor::TraverseDecl(clang::Decl *d)
{
    ScopedSwitcher<Context> sw1(m_thisContext, 0);
    ScopedSwitcher<Context> sw2(m_childContext, 0);

	std::shared_ptr<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>> sw3;
	if (d && clang::isa<clang::DeclContext>(d) && clang::isa<clang::NamedDecl>(d) && !clang::isa<clang::NamespaceDecl>(d))
	{
		clang::NamedDecl* nd = clang::dyn_cast<clang::NamedDecl>(d);
		sw3 = std::make_shared<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>>(
			m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(nd, m_declNameCache)
		);
	}

    return base::TraverseDecl(d);
}

bool ASTVisitor::TraverseLambdaExpr(clang::LambdaExpr* e)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(e->getCallOperator(), m_declNameCache)
	);
	return base::TraverseLambdaExpr(e);
}

bool ASTVisitor::TraverseFunctionDecl(clang::FunctionDecl* d)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_childContextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
	); // store context for template arguments of function specialitzation
	return base::TraverseFunctionDecl(d);
}

bool ASTVisitor::TraverseTypedefDecl(clang::TypedefDecl *d)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
	);
	return base::TraverseTypedefDecl(d);
}

bool ASTVisitor::TraverseFieldDecl(clang::FieldDecl *d)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
	);
	return base::TraverseFieldDecl(d);
}

bool ASTVisitor::TraverseVarDecl(clang::VarDecl *d)
{
	std::shared_ptr<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>> switcher;

	NameHierarchy contextNameHierarchy = getContextName();
	if (!(contextNameHierarchy.size() > 0 && contextNameHierarchy.back()->hasSignature())) // TODO: whle test if its a function. optimize this: remove requirement to get the name here!
	{
		switcher = std::make_shared<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>>(
			m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
		);
	}
	return base::TraverseVarDecl(d);
}

bool ASTVisitor::TraverseClassTemplateDecl(clang::ClassTemplateDecl* d)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
	);
	return base::TraverseClassTemplateDecl(d);
}

bool ASTVisitor::TraverseFunctionTemplateDecl(clang::FunctionTemplateDecl* d)
{
	// we need to use the templated decl here because name resolving for FunctionTemplateDecl is not returning a correct signature yet.
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d->getTemplatedDecl(), m_declNameCache)
	);
	return base::TraverseFunctionTemplateDecl(d);
}

bool ASTVisitor::TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d)
{
	// same as base::TraverseTemplateTypeParmDecl(..) but we need to integrate the setter for the context info.
	WalkUpFromTemplateTypeParmDecl(d);

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		ScopedSwitcher<RefType> sw1(m_typeContext, RT_TemplateDefaultArgument);
		ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> sw2(
			m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
		);
		TraverseTypeLoc(d->getDefaultArgumentInfo()->getTypeLoc());
	}

	traverseDeclContextHelper(clang::dyn_cast<clang::DeclContext>(d));
	return true;
}

bool ASTVisitor::TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d)
{
	// same as base::TraverseTemplateTemplateParmDecl(..) but we need to integrate the setter for the context info.
	WalkUpFromTemplateTemplateParmDecl(d);

	TraverseDecl(d->getTemplatedDecl());

	if (d->hasDefaultArgument() && !d->defaultArgumentWasInherited())
	{
		ScopedSwitcher<RefType> sw1(m_typeContext, RT_TemplateDefaultArgument);
		ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> sw2(
			m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
		);

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

bool ASTVisitor::TraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_childContextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
	);
	return base::TraverseClassTemplatePartialSpecializationDecl(d);
}

bool ASTVisitor::TraverseDeclRefExpr(clang::DeclRefExpr* e)
{
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_childContextNameGenerator, std::make_shared<ContextDeclNameGenerator>(e->getDecl(), m_declNameCache)
	);
	return base::TraverseDeclRefExpr(e);
}

bool ASTVisitor::TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc)
{
	const clang::Type* t = loc.getTypePtr();
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
		m_childContextNameGenerator, std::make_shared<ContextTypeNameGenerator>(t, m_typeNameCache)
	);
	return base::TraverseTemplateSpecializationTypeLoc(loc);
}

bool ASTVisitor::TraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) // TODO: do this for unresolved and dependent stuff
{
	std::shared_ptr<ContextNameGenerator> clear;
	ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> sw(m_childContextNameGenerator, clear);
	return base::TraverseUnresolvedLookupExpr(e);
}

bool ASTVisitor::TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	std::shared_ptr<ScopedSwitcher<RefType>> sw1;
	std::shared_ptr<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>> sw2;

	if (m_typeContext != RT_TemplateDefaultArgument
		&& m_childContextNameGenerator)
	{
		sw1 = std::make_shared<ScopedSwitcher<RefType>>(m_typeContext, RT_TemplateArgument);
		sw2 = std::make_shared<ScopedSwitcher<std::shared_ptr<ContextNameGenerator>>>(
			m_contextNameGenerator, m_childContextNameGenerator
		);
	}

	clang::TemplateArgument::ArgKind kk = loc.getArgument().getKind();
	if (kk == clang::TemplateArgument::Template)
	{
		RecordDeclRef(loc.getArgument().getAsTemplate().getAsTemplateDecl(), loc.getLocation(), m_typeContext);
	}

	return base::TraverseTemplateArgumentLoc(loc);
}

///////////////////////////////////////////////////////////////////////////////
// Expression context propagation

bool ASTVisitor::TraverseCallCommon(clang::CallExpr *call)
{
    {
        m_childContext = CF_Called;
        TraverseStmt(call->getCallee());
    }
    for (unsigned int i = 0; i < call->getNumArgs(); ++i) {
        clang::Expr *arg = call->getArg(i);
        // If the child is really an lvalue, then this call is passing a
        // reference.  If the child is not an lvalue, then this address-taken
        // flag will be masked out, and the rvalue will be assumed to be read.
        m_childContext = CF_AddressTaken;
        TraverseStmt(arg);
    }
    return true;
}

bool ASTVisitor::TraverseBinComma(clang::BinaryOperator *s)
{
    {
        m_childContext = 0;
        TraverseStmt(s->getLHS());
    }
    {
        m_childContext = m_thisContext;
        TraverseStmt(s->getRHS());
    }
    return true;
}

bool ASTVisitor::TraverseAssignCommon(
        clang::BinaryOperator *e,
        ContextFlags lhsFlag)
{
    {
        m_childContext = m_thisContext | lhsFlag;
        if (m_childContext & CF_Called) {
            m_childContext ^= CF_Called;
            m_childContext |= CF_Read;
        }
        TraverseStmt(e->getLHS());
    }
    {
        m_childContext = 0;
        TraverseStmt(e->getRHS());
    }
    return true;
}

bool ASTVisitor::VisitCastExpr(clang::CastExpr *e)
{
    if (e->getCastKind() == clang::CK_ArrayToPointerDecay) {
        // Note that e->getSubExpr() can be an rvalue array, in which case the
        // CF_AddressTaken context will be masked away to 0.
        m_childContext = CF_AddressTaken;
    } else if (e->getCastKind() == clang::CK_ToVoid) {
        m_childContext = 0;
    } else if (e->getCastKind() == clang::CK_LValueToRValue) {
        m_childContext = CF_Read;
    }
    return true;
}

bool ASTVisitor::VisitUnaryAddrOf(clang::UnaryOperator *e)
{
    m_childContext = CF_AddressTaken;
    return true;
}

bool ASTVisitor::VisitUnaryDeref(clang::UnaryOperator *e)
{
    m_childContext = 0;
    return true;
}

bool ASTVisitor::VisitDeclStmt(clang::DeclStmt *s)
{
    // If a declaration is a reference to an lvalue initializer, then we
    // record that that the initializer's address was taken.  If it is
    // an rvalue instead, then we'll see something else in the tree indicating
    // what kind of reference to record (e.g. lval-to-rval cast, assignment,
    // call, & operator) or assume the rvalue is being read.
    m_childContext = CF_AddressTaken;
    return true;
}

bool ASTVisitor::VisitReturnStmt(clang::ReturnStmt *s)
{
    // See comment for VisitDeclStmt.
    m_childContext = CF_AddressTaken;
    return true;
}

bool ASTVisitor::VisitVarDecl(clang::VarDecl *d)
{
    // See comment for VisitDeclStmt.  Set the context for the variable's
    // initializer.  Also handle default arguments on parameter declarations.
    m_childContext = CF_AddressTaken;
    return true;
}

bool ASTVisitor::VisitInitListExpr(clang::InitListExpr *e)
{
    // See comment for VisitDeclStmt.  An initializer list can also bind
    // references.
    m_childContext = CF_AddressTaken;
    return true;
}

bool ASTVisitor::TraverseConstructorInitializer(clang::CXXCtorInitializer *init)
{
    if (init->getMember() != NULL)
	{
        RecordDeclRef(init->getMember(),
                      init->getMemberLocation(),
                      RT_Initialized,
					  SYMBOL_FIELD);
    }

    // See comment for VisitDeclStmt.
    m_childContext = CF_AddressTaken;
    return base::TraverseConstructorInitializer(init);
}


///////////////////////////////////////////////////////////////////////////////
// Expression reference recording

bool ASTVisitor::VisitLambdaExpr(clang::LambdaExpr* e)
{
	RecordDeclRef(e->getCallOperator(), e->getLocStart(), RT_Definition, SYMBOL_FUNCTION);
	return true;
}

bool ASTVisitor::VisitMemberExpr(clang::MemberExpr *e)
{
    RecordDeclRefExpr(
                e->getMemberDecl(),
                e->getMemberLoc(),
                e,
                m_thisContext);

    // Update the child context for the base sub-expression.
    if (e->isArrow()) {
        m_childContext = CF_Read;
    } else {
        m_childContext = 0;
        if (m_thisContext & CF_AddressTaken)
            m_childContext |= CF_AddressTaken;
        if (m_thisContext & (CF_Assigned | CF_Modified))
            m_childContext |= CF_Modified;
        if (m_thisContext & CF_Read)
            m_childContext |= CF_Read;
        if (m_thisContext & CF_Called) {
            // I'm not sure what the best behavior here is.
            m_childContext |= CF_Read;
        }
    }

    return true;
}

bool ASTVisitor::VisitDeclRefExpr(clang::DeclRefExpr *e)
{
    RecordDeclRefExpr(
                e->getDecl(),
                e->getLocation(),
                e,
                m_thisContext);
    return true;
}

bool ASTVisitor::VisitCXXConstructExpr(clang::CXXConstructExpr *e)
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
	clang::SourceLocation braceBeginLoc = e->getParenOrBraceRange().getBegin();
	clang::SourceLocation nameBeginLoc = e->getSourceRange().getBegin();
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
		loc = e->getSourceRange().getEnd();
	}
	loc = clang::Lexer::GetBeginningOfToken(loc, m_context->getSourceManager(), m_context->getLangOpts());
	RecordDeclRefExpr(
		e->getConstructor(),
		loc,
		e,
		CF_Called);
    return true;
}

void ASTVisitor::RecordDeclRefExpr(clang::NamedDecl *d, clang::SourceLocation loc, clang::Expr *e, Context context)
{
	SymbolType symbolType = SYMBOL_TYPE_MAX;

	if (clang::isa<clang::VarDecl>(d))
	{
		if (llvm::isa<clang::ParmVarDecl>(d))
		{
			symbolType = SYMBOL_PARAMETER;
		}
		else if (d->getParentFunctionOrMethod() == NULL)
		{
			symbolType = SYMBOL_GLOBAL_VARIABLE;
		}
		else
		{
			symbolType = SYMBOL_LOCAL_VARIABLE;
		}
	}
	if (clang::isa<clang::EnumConstantDecl>(d))
	{
		symbolType = SYMBOL_ENUM_CONSTANT;
	}
	else if (clang::isa<clang::FieldDecl>(d))
	{
		symbolType = SYMBOL_FIELD;
	}

	if (m_typeContext == RT_TemplateArgument)
	{
		RecordDeclRef(d, loc, m_typeContext, symbolType);
	}
	else
	{

		if (llvm::isa<clang::FunctionDecl>(*d))
		{
			// XXX: This code seems sloppy, but I suspect it will work well enough.
			if (context & CF_Called)
				RecordDeclRef(d, loc, RT_Called, symbolType);
			if (!(context & CF_Called) || (context & (CF_Read | CF_AddressTaken)))
				RecordDeclRef(d, loc, RT_AddressTaken, symbolType);
		}
		else
		{
			if (context & CF_Called)
				RecordDeclRef(d, loc, RT_Called, symbolType);
			if (context & CF_Read)
				RecordDeclRef(d, loc, RT_Read, symbolType);
			if (context & CF_AddressTaken)
				RecordDeclRef(d, loc, RT_AddressTaken, symbolType);
			if (context & CF_Assigned)
				RecordDeclRef(d, loc, RT_Assigned, symbolType);
			if (context & CF_Modified)
				RecordDeclRef(d, loc, RT_Modified, symbolType);
			if (context == 0)
				RecordDeclRef(d, loc, e->isRValue() ? RT_Read : RT_Other, symbolType);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// NestedNameSpecifier handling

bool ASTVisitor::TraverseNestedNameSpecifierLoc(
        clang::NestedNameSpecifierLoc qualifier)
{
    for (; qualifier; qualifier = qualifier.getPrefix()) {
        clang::NestedNameSpecifier *nns = qualifier.getNestedNameSpecifier();
        switch (nns->getKind()) {
        case clang::NestedNameSpecifier::Namespace:
            RecordDeclRef(nns->getAsNamespace(),
                          qualifier.getLocalBeginLoc(),
                          RT_Qualifier);
            break;
        case clang::NestedNameSpecifier::NamespaceAlias:
            RecordDeclRef(nns->getAsNamespaceAlias(),
                          qualifier.getLocalBeginLoc(),
                          RT_Qualifier);
            break;
        case clang::NestedNameSpecifier::TypeSpec:
        case clang::NestedNameSpecifier::TypeSpecWithTemplate:
            if (const clang::TypedefType *tt = nns->getAsType()->getAs<clang::TypedefType>()) {
                RecordDeclRef(tt->getDecl(),
                              qualifier.getLocalBeginLoc(),
                              RT_Qualifier);
            } else if (const clang::RecordType *rt = nns->getAsType()->getAs<clang::RecordType>()) {
                RecordDeclRef(rt->getDecl(),
                              qualifier.getLocalBeginLoc(),
                              RT_Qualifier);
            } else if (const clang::TemplateSpecializationType *tst =
                       nns->getAsType()->getAs<clang::TemplateSpecializationType>()) {

                if (clang::TemplateDecl *decl = tst->getTemplateName().getAsTemplateDecl()) {
                    if (clang::NamedDecl *templatedDecl = decl->getTemplatedDecl()) {
                        RecordDeclRef(templatedDecl,
                                      qualifier.getLocalBeginLoc(),
                                      RT_Qualifier);

                    }
                }
            }
            break;
        default: // case Global: case Super:
            // do nothing for the remaining cases
            break;
        }
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Declaration and TypeLoc handling

void ASTVisitor::traverseDeclContextHelper(clang::DeclContext *d)
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

// Overriding TraverseCXXRecordDecl lets us mark the base-class references
// with the "Base-Class" kind.
bool ASTVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl *d)
{
	if (d->isImplicit())
	{
		// do nothing
		return true;
	}

    // Traverse qualifiers on the record decl.
    TraverseNestedNameSpecifierLoc(d->getQualifierLoc());

    // Visit the TagDecl to record its ref.
    WalkUpFromCXXRecordDecl(d);

    // Traverse base classes.
    if (d->isThisDeclarationADefinition()) {
        for (clang::CXXRecordDecl::base_class_iterator it = d->bases_begin();
                it != d->bases_end();
                ++it) {
            clang::CXXBaseSpecifier *baseSpecifier = it;
			ScopedSwitcher<RefType> sw1(m_typeContext, RT_BaseClass);
			ScopedSwitcher<ParserClient::AccessType> sw2(
				m_contextAccess, convertAccessType(baseSpecifier->getAccessSpecifier())
			);
			ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> sw3(
				m_contextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
			);
            TraverseTypeLoc(baseSpecifier->getTypeSourceInfo()->getTypeLoc());
        }
    }

    traverseDeclContextHelper(d);
    return true;
}

bool ASTVisitor::TraverseClassTemplateSpecializationDecl(
        clang::ClassTemplateSpecializationDecl *d)
{
    // base::TraverseClassTemplateSpecializationDecl calls TraverseTypeLoc,
    // which then visits a clang::TemplateSpecializationTypeLoc.  We want
    // to mark the template specialization as Declaration or Definition,
    // not Reference, so skip the TraverseTypeLoc call.
    //
    // The problem happens with code like this:
    //     template <>
    //     struct Vector<bool, void> {};

    WalkUpFromClassTemplateSpecializationDecl(d);

	if (clang::TypeSourceInfo* tsi = d->getTypeAsWritten())
	{
		ScopedSwitcher<std::shared_ptr<ContextNameGenerator>> switcher(
			m_childContextNameGenerator, std::make_shared<ContextDeclNameGenerator>(d, m_declNameCache)
		);
		clang::TypeLoc tl = tsi->getTypeLoc();
		clang::TemplateSpecializationTypeLoc tstl = tl.castAs<clang::TemplateSpecializationTypeLoc>();
		for (unsigned I = 0, E = tstl.getNumArgs(); I != E; ++I)
		{
			TraverseTemplateArgumentLoc(tstl.getArgLoc(I));
		}
	}

    traverseDeclContextHelper(d);



    return true;
	//base::TraverseClassTemplateSpecializationDecl(d);
}

bool ASTVisitor::TraverseNamespaceAliasDecl(clang::NamespaceAliasDecl *d)
{
    // The base::TraverseNamespaceAliasDecl function avoids traversing the
    // namespace decl itself because (I think) that would traverse the
    // complete contents of the namespace.  However, it fails to traverse
    // the qualifiers on the target namespace, so we do that here.
    TraverseNestedNameSpecifierLoc(d->getQualifierLoc());

    return base::TraverseNamespaceAliasDecl(d);
}

void ASTVisitor::templateParameterListsHelper(clang::DeclaratorDecl *d)
{
    for (unsigned i = 0, iEnd = d->getNumTemplateParameterLists();
            i != iEnd; ++i) {
        clang::TemplateParameterList *parmList =
                d->getTemplateParameterList(i);
        for (clang::NamedDecl *parm : *parmList)
            TraverseDecl(parm);
    }
}

bool ASTVisitor::VisitDecl(clang::Decl *d)
{
    if (clang::NamedDecl *nd = llvm::dyn_cast<clang::NamedDecl>(d)) {
        clang::SourceLocation loc = nd->getLocation();
        if (clang::FunctionDecl *fd = llvm::dyn_cast<clang::FunctionDecl>(d)) {
            //if (fd->getTemplateInstantiationPattern() != NULL) {
            //    // When Clang instantiates a function template, it seems to
            //    // create a FunctionDecl for the instantiation that returns
            //    // false for fd->isThisDeclarationADefinition().  The result
            //    // is that the template function definition's location is
            //    // marked as both a Declaration and a Definition.  Fix this by
            //    // omitting the ref on the instantiation.
            //} else {
#if 0
                // This code recorded refs without appropriate qualifiers.  For
                // example, with the code
                //     template <typename A> void Vector<A>::clear() {}
                // it would record the first A as "A", but it needs to record
                // Vector::A.
                templateParameterListsHelper(fd);
#endif
                RefType refType;
                refType = fd->isThisDeclarationADefinition() ?
                            RT_Definition : RT_Declaration;
                SymbolType symbolType;
                if (llvm::isa<clang::CXXMethodDecl>(fd))
				{
					symbolType = SYMBOL_METHOD;
                }
				else
				{
					symbolType = SYMBOL_FUNCTION;
                }
                RecordDeclRef(nd, loc, refType, symbolType);
				if (fd->isFunctionTemplateSpecialization())
				{
					RecordDeclRef(nd, loc, RT_TemplateSpecialization, symbolType);
				}
            //}
        } else if (clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(d)) {
            // Don't record the parameter definitions in a function declaration
            // (unless the function declaration is also a definition).  A
            // definition will be recorded at the function's definition, and
            // recording two definitions is unhelpful.  This code could record
            // a different kind of reference, but recording the position of
            // parameter names in declarations doesn't seem useful.
            bool omitParamVar = false;
            const bool isParam = llvm::isa<clang::ParmVarDecl>(vd);
            if (isParam) {
                clang::FunctionDecl *fd =
                        llvm::dyn_cast_or_null<clang::FunctionDecl>(
                            vd->getDeclContext());
                if (fd && !fd->isThisDeclarationADefinition())
                    omitParamVar = true;
            }
            if (!omitParamVar) {
                RefType refType;
                if (vd->isThisDeclarationADefinition() == clang::VarDecl::DeclarationOnly)
                    refType = RT_Declaration;
                else
                    refType = RT_Definition;
                // TODO: Review for correctness.  What about local extern?
                SymbolType symbolType;
				if (isParam)
				{
					symbolType = SYMBOL_PARAMETER;
				}
				else if (vd->getParentFunctionOrMethod() == NULL)
				{
					if (vd->getAccess() == clang::AS_none)
					{
						symbolType = SYMBOL_GLOBAL_VARIABLE;
					}
					else
					{
						symbolType = SYMBOL_FIELD;
					}
				}
				else
				{
					symbolType = SYMBOL_LOCAL_VARIABLE;
				}
                RecordDeclRef(nd, loc, refType, symbolType);
            }
        } else if (clang::TagDecl *td = llvm::dyn_cast<clang::TagDecl>(d)) {
            RefType refType;
            refType = td->isThisDeclarationADefinition() ? RT_Definition : RT_Declaration;
            // Mark an extern template declaration as a Declaration rather than
            // a Definition.  For example:
            //     template<typename T> class Foo {};  // Definition of Foo
            //     extern template class Foo<int>;     // Declaration of Foo
            if (clang::ClassTemplateSpecializationDecl *spec =
                    llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(td)) {
                if (spec->getTemplateSpecializationKind() !=
                        clang::TSK_ExplicitSpecialization)
                    refType = RT_Declaration;
            }

			SymbolType symbolType = SYMBOL_TYPE_MAX;
			// TODO: Handle the C++11 fixed underlying type of enumeration
			// declarations.
			switch (td->getTagKind())
			{
				case clang::TTK_Struct: symbolType = SYMBOL_STRUCT; break;
				case clang::TTK_Union:  symbolType = SYMBOL_UNION; break;
				case clang::TTK_Class:  symbolType = SYMBOL_CLASS; break;
				case clang::TTK_Enum:   symbolType = SYMBOL_ENUM; break;
				default: assert(false);
			}
            RecordDeclRef(nd, loc, refType, symbolType);

			if (clang::isa<clang::ClassTemplateSpecializationDecl>(td))
			{
				RecordDeclRef(nd, loc, RT_TemplateSpecialization, symbolType);
			}
        } else if (clang::UsingDirectiveDecl *ud = llvm::dyn_cast<clang::UsingDirectiveDecl>(d)) {
            RecordDeclRef(
                        ud->getNominatedNamespaceAsWritten(),
                        loc, RT_UsingDirective);
        } else if (clang::UsingDecl *usd = llvm::dyn_cast<clang::UsingDecl>(d)) {
            for (auto it = usd->shadow_begin(), itEnd = usd->shadow_end();
                    it != itEnd; ++it) {
                clang::UsingShadowDecl *shadow = *it;
                RecordDeclRef(shadow->getTargetDecl(), loc, RT_Using);
            }
        } else if (clang::NamespaceAliasDecl *nad = llvm::dyn_cast<clang::NamespaceAliasDecl>(d)) {
			RecordDeclRef(nad, loc, RT_Declaration, SYMBOL_NAMESPACE);
//			not needed right now!
//          RecordDeclRef(nad, loc, RT_Declaration, ST_Namespace);
// 			RecordDeclRef(nad->getAliasedNamespace(),
//                          nad->getTargetNameLoc(),
//                          RT_NamespaceAlias);
        } else if (llvm::isa<clang::FunctionTemplateDecl>(d)) {
			// TODO: use these cases for creating the connection between two undefined template things
			// Do nothing.  The function will be recorded when it appears as a
            // FunctionDecl.
        } else if (llvm::isa<clang::ClassTemplateDecl>(d)) {
            // Do nothing.  The class will be recorded when it appears as a
            // RecordDecl.
        } else if (llvm::isa<clang::FieldDecl>(d)) {
            RecordDeclRef(nd, loc, RT_Declaration, SYMBOL_FIELD);
        } else if (llvm::isa<clang::TypedefDecl>(d)) {
            RecordDeclRef(nd, loc, RT_Declaration, SYMBOL_TYPEDEF);
        } else if (llvm::isa<clang::NamespaceDecl>(d)) {
            RecordDeclRef(nd, loc, RT_Declaration, SYMBOL_NAMESPACE);
        } else if (llvm::isa<clang::EnumConstantDecl>(d)) {
            RecordDeclRef(nd, loc, RT_Declaration, SYMBOL_ENUM_CONSTANT);
		} else if (
			llvm::isa<clang::NonTypeTemplateParmDecl>(d) ||
			llvm::isa<clang::TemplateTypeParmDecl>(d) ||
			llvm::isa<clang::TemplateTemplateParmDecl>(d)) {
			RecordDeclRef(nd, loc, RT_Declaration, SYMBOL_TEMPLATE_PARAMETER);
        } else {
            RecordDeclRef(nd, loc, RT_Declaration);
        }
    }

    return true;
}

//#include "data/parser/ParseFunction.h"
//#include "data/parser/cxx/name_resolver/CxxTypeNameResolver.h"
//#include "data/type/NamedDataType.h"

bool ASTVisitor::VisitTypeLoc(clang::TypeLoc tl)
{
	if (!tl.getAs<clang::TagTypeLoc>().isNull())
	{
		const clang::TagTypeLoc &ttl = tl.castAs<clang::TagTypeLoc>();
		RecordDeclRef(ttl.getDecl(),
			tl.getBeginLoc(),
			m_typeContext);
	}
	else if (!tl.getAs<clang::TypedefTypeLoc>().isNull())
	{
		const clang::TypedefTypeLoc &ttl = tl.castAs<clang::TypedefTypeLoc>();
		RecordDeclRef(ttl.getTypedefNameDecl(),
			tl.getBeginLoc(),
			m_typeContext);
	}
	else if (!tl.getAs<clang::TemplateTypeParmTypeLoc>().isNull())
	{
		const clang::TemplateTypeParmTypeLoc &ttptl =
		tl.castAs<clang::TemplateTypeParmTypeLoc>();
		RecordDeclRef(ttptl.getDecl(),
			tl.getBeginLoc(),
			m_typeContext);
	}
	else if (!tl.getAs<clang::TemplateSpecializationTypeLoc>().isNull())
	{
		const clang::TemplateSpecializationTypeLoc &tstl =
			tl.castAs<clang::TemplateSpecializationTypeLoc>();
		const clang::TemplateSpecializationType &tst =
			*tstl.getTypePtr()->getAs<clang::TemplateSpecializationType>();
		if (tst.getAsCXXRecordDecl())
		{
			RecordDeclRef(tst.getAsCXXRecordDecl(),
				tl.getBeginLoc(),
				m_typeContext);
		}
		else	// if template specialization cannot be resolved to a concrete declaration.
		{		// this is the case when using a specialization of a template template parameter.
			RecordTypeRef(tstl.getTypePtr(),
				tl.getBeginLoc(),
				m_typeContext);
		}
	}
	else if (!tl.getAs<clang::DependentNameTypeLoc>().isNull())
	{
		const clang::DependentNameTypeLoc& dntl =
			tl.castAs<clang::DependentNameTypeLoc>();
		RecordTypeRef(dntl.getTypePtr(),
			dntl.getNameLoc(),
			m_typeContext);
	}
	else if (!tl.getAs<clang::BuiltinTypeLoc>().isNull())
	{
		const clang::BuiltinTypeLoc &btl =
			tl.castAs<clang::BuiltinTypeLoc>();
		RecordTypeRef(btl.getTypePtr(),
			tl.getBeginLoc(),
			m_typeContext);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Reference recording

// static inline bool isNamedDeclUnnamed(clang::NamedDecl *d)
// {
//     return d->getDeclName().isIdentifier() && d->getIdentifier() == NULL;
// }

ParseLocation ASTVisitor::getDeclRefRange(clang::NamedDecl *decl, clang::SourceLocation loc)
{
	ParseLocation parseLocation;
	clang::SourceManager& sourceManager = m_context->getSourceManager();
	clang::SourceLocation sloc = sourceManager.getSpellingLoc(loc);
	{
		clang::FileID fileId = sourceManager.getFileID(sloc);

		if (!fileId.isInvalid())
		{
			const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
			if (fileEntry != NULL)
			{
				parseLocation.filePath = FilePath(fileEntry->getName()).canonical();
			}
		}

		unsigned int offset = sourceManager.getFileOffset(sloc);
		parseLocation.startLineNumber = sourceManager.getLineNumber(fileId, offset);
		parseLocation.startColumnNumber = sourceManager.getColumnNumber(fileId, offset);
	}
	if (decl)
	{
		clang::DeclarationName name = decl->getDeclName();
		clang::DeclarationName::NameKind nameKind = name.getNameKind();

		// A C++ destructor name consists of two tokens, '~' and an identifier.
		// Try to include both of them in the ref.
		if (nameKind == clang::DeclarationName::CXXDestructorName) {
			// Start by getting the destructor name, sans template arguments.
			const clang::Type *nameType = name.getCXXNameType().getTypePtr();
			assert(nameType != NULL);
			llvm::StringRef className;
			if (const clang::InjectedClassNameType *injectedNameType =
				nameType->getAs<clang::InjectedClassNameType>()) {
				className = injectedNameType->getDecl()->getName();
			}
			else if (const clang::RecordType *recordType =
				nameType->getAs<clang::RecordType>()) {
				className = recordType->getDecl()->getName();
			}
			if (!className.empty()) { // TODO: maybe its better to use tokens here.
				// Scan the characters.
				const char *const buffer = sourceManager.getCharacterData(sloc);
				const char *p = buffer;
				if (p != NULL && *p == '~') {
					p++;
					// Permit whitespace between the ~ and the class name.
					// Technically there could be preprocessor tokens, comments,
					// etc..
					while (*p == ' ' || *p == '\t')
						p++;
					// Match the class name against the text in the source.
					if (!strncmp(p, className.data(), className.size())) {
						p += className.size();
						if (!isalnum(*p) && *p != '_') {
							parseLocation.endLineNumber = parseLocation.startLineNumber;
							parseLocation.endColumnNumber = parseLocation.startColumnNumber;
							parseLocation.endColumnNumber += p - buffer - 1;
							return parseLocation;
						}
					}
				}
			}
		}

		// For references to C++ overloaded operators, try to include both the
		// operator keyword and the operator name in the ref.
		if (nameKind == clang::DeclarationName::CXXOperatorName) {
			const char *spelling = clang::getOperatorSpelling(
				name.getCXXOverloadedOperator());
			if (spelling != NULL) {
				const char *const buffer = sourceManager.getCharacterData(sloc);
				const char *p = buffer;
				if (p != NULL && !strncmp(p, "operator", 8)) {
					p += 8;
					// Skip whitespace between "operator" and the operator itself.
					while (*p == ' ' || *p == '\t')
						p++;
					// Look for the operator name.  This may be too restrictive for
					// recognizing multi-token operators like operator[], operator
					// delete[], or operator ->*.
					if (!strncmp(p, spelling, strlen(spelling))) {
						p += strlen(spelling);
						parseLocation.endLineNumber = parseLocation.startLineNumber;
						parseLocation.endColumnNumber = parseLocation.startColumnNumber;
						parseLocation.endColumnNumber += p - buffer - 1;
						return parseLocation;
					}
				}
			}
		}
	}
    // General case -- find the end of the token starting at loc.

	{
		clang::SourceLocation endSloc =
			m_preprocessor->getLocForEndOfToken(sloc);

		unsigned int offset = sourceManager.getFileOffset(endSloc);
		clang::FileID fileId = sourceManager.getFileID(endSloc);
		parseLocation.endLineNumber = sourceManager.getLineNumber(fileId, offset);
		parseLocation.endColumnNumber = sourceManager.getColumnNumber(fileId, offset) - 1;
	}
	return parseLocation;
}

void ASTVisitor::RecordTypeRef(
	const clang::Type* type,
	clang::SourceLocation beginLoc,
	RefType refType,
	SymbolType symbolType)
{
	if (isLocatedInUnparsedProjectFile(beginLoc))
	{
		ParseLocation parseLocation = getDeclRefRange(0, beginLoc);
		NameHierarchy typeNameHierarchy = m_typeNameCache->getValue(type);
		NameHierarchy contextNameHierarchy = getContextName();

		if (refType == RT_TemplateArgument)
		{
			m_client->onTemplateArgumentTypeParsed(
				parseLocation, typeNameHierarchy, contextNameHierarchy);
		}
		else if (refType == RT_BaseClass)
		{
			m_client->onInheritanceParsed(
				parseLocation, contextNameHierarchy, typeNameHierarchy, m_contextAccess);
		}
		else if (refType == RT_TemplateDefaultArgument)
		{
			m_client->onTemplateDefaultArgumentTypeParsed(
				parseLocation, typeNameHierarchy, contextNameHierarchy);
		}
		else
		{
			m_client->onTypeUsageParsed(
				parseLocation, contextNameHierarchy, typeNameHierarchy);
		}
	}
}

void ASTVisitor::RecordDeclRef(
        clang::NamedDecl* d,
        clang::SourceLocation beginLoc,
        RefType refType,
        SymbolType symbolType)
{
	bool declIsImplicit = isImplicit(d);

	if (!d ||
		(declIsImplicit && !isLocatedInProjectFile(beginLoc)) ||
		(!declIsImplicit && !isLocatedInUnparsedProjectFile(beginLoc)))
	{
		return;
	}

	ParseLocation parseLocation = getDeclRefRange(d, beginLoc);
	NameHierarchy declNameHierarchy = m_declNameCache->getValue(d);

	bool fallback = false;


	if (symbolType == SYMBOL_LOCAL_VARIABLE || symbolType == SYMBOL_PARAMETER)
	{
		if (!declIsImplicit)
		{
			if (clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(d))
			{
				ParseLocation declLocation = getParseLocation(varDecl->getSourceRange());
				std::string name =
					declLocation.filePath.str() + "::" +
					varDecl->getNameAsString() + "<" +
					std::to_string(declLocation.startLineNumber) + ":" +
					std::to_string(declLocation.startColumnNumber) + ">";
				m_client->onLocalSymbolParsed(
					name,
					parseLocation
				);
			}
		}
		return;
	}

	switch (refType)
	{
	case RT_Declaration:
	case RT_Definition:
		{
			switch (symbolType)
			{
			case SYMBOL_TYPEDEF:
				if (clang::TypedefDecl* typedefDecl = clang::dyn_cast<clang::TypedefDecl>(d))
				{
					m_client->onTypedefParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(typedefDecl->getAccess()),
						declIsImplicit);
				}
				break;
			case SYMBOL_CLASS:
				if (clang::RecordDecl* recordDecl = clang::dyn_cast<clang::RecordDecl>(d))
				{
					m_client->onClassParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(recordDecl->getAccess()),
						(refType == RT_Definition ? getParseLocationOfRecordBody(recordDecl) : ParseLocation()),
						declIsImplicit);
				}
				break;
			case SYMBOL_STRUCT:
				if (clang::RecordDecl* recordDecl = clang::dyn_cast<clang::RecordDecl>(d))
				{
					m_client->onStructParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(recordDecl->getAccess()),
						(refType == RT_Definition ? getParseLocationOfRecordBody(recordDecl) : ParseLocation()),
						declIsImplicit);
				}
				break;
			case SYMBOL_GLOBAL_VARIABLE:
				m_client->onGlobalVariableParsed(
					parseLocation,
					declNameHierarchy,
					declIsImplicit);
				break;
			case SYMBOL_FIELD:
				//if (clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(d))
				{
					m_client->onFieldParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(d->getAccess()),
						declIsImplicit);
				}
				break;
			case SYMBOL_FUNCTION:
				if (clang::FunctionDecl* functionDecl = clang::dyn_cast<clang::FunctionDecl>(d))
				{
					m_client->onFunctionParsed(
						parseLocation,
						declNameHierarchy,
						(refType == RT_Definition ? getParseLocationOfFunctionBody(functionDecl) : ParseLocation()),
						declIsImplicit);
				}
				break;
			case SYMBOL_METHOD:
				if (clang::CXXMethodDecl* methodDecl = clang::dyn_cast<clang::CXXMethodDecl>(d))
				{
					m_client->onMethodParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(methodDecl->getAccess()),
						getAbstractionType(methodDecl),
						(refType == RT_Definition ? getParseLocationOfFunctionBody(methodDecl) : ParseLocation()),
						declIsImplicit);

					for (clang::CXXMethodDecl::method_iterator it = methodDecl->begin_overridden_methods(); // iterate in traversal and use RT_Overridden or so..
						it != methodDecl->end_overridden_methods(); it++)
					{
						m_client->onMethodOverrideParsed(
							parseLocation,
							m_declNameCache->getValue(*it),
							declNameHierarchy);
					}

					clang::MemberSpecializationInfo* memberSpecializationInfo = methodDecl->getMemberSpecializationInfo();
					if (memberSpecializationInfo)
					{
						clang::NamedDecl* specializedNamedDecl = memberSpecializationInfo->getInstantiatedFrom();
						if (clang::isa<clang::FunctionDecl>(specializedNamedDecl))
						{
							m_client->onTemplateMemberFunctionSpecializationParsed(
								parseLocation,
								declNameHierarchy,
								m_declNameCache->getValue(specializedNamedDecl));
						}
					}
				}
				break;
			case SYMBOL_NAMESPACE:
				if (clang::NamespaceDecl* namespaceDecl = clang::dyn_cast<clang::NamespaceDecl>(d))
				{
					m_client->onNamespaceParsed(
						namespaceDecl->isAnonymousNamespace() ? ParseLocation() : parseLocation,
						declNameHierarchy,
						getParseLocation(namespaceDecl->getSourceRange()),
						declIsImplicit);
				}
				else if (clang::NamespaceAliasDecl* namespaceAliasDecl = clang::dyn_cast<clang::NamespaceAliasDecl>(d))
				{
					m_client->onNamespaceParsed(
						parseLocation,
						declNameHierarchy,
						getParseLocation(namespaceAliasDecl->getAliasedNamespace()->getSourceRange()),
						declIsImplicit);
				}
				break;
			case SYMBOL_ENUM:
				if (clang::EnumDecl* enumDecl = clang::dyn_cast<clang::EnumDecl>(d))
				{
					m_client->onEnumParsed(
						parseLocation,
						declNameHierarchy,
						convertAccessType(enumDecl->getAccess()),
						getParseLocation(enumDecl->getSourceRange()),
						declIsImplicit);
				}
				break;
			case SYMBOL_ENUM_CONSTANT:
				m_client->onEnumConstantParsed(
					parseLocation,
					declNameHierarchy,
					declIsImplicit);
				break;
			case SYMBOL_TEMPLATE_PARAMETER:
				if (!d->getName().empty()) // We don't create symbols for unnamed template parameters.
				{
					m_client->onTemplateParameterTypeParsed(
						parseLocation,
						declNameHierarchy,
						declIsImplicit);
				}
				break;
			default:
				fallback = true;
				break;
			}
			break;
		}
	case RT_TemplateSpecialization:
		{
			if (clang::ClassTemplateSpecializationDecl* classTemplateSpecializationDecl = clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(d))
			{
				clang::NamedDecl* specializedFromDecl;
				llvm::PointerUnion<clang::ClassTemplateDecl*, clang::ClassTemplatePartialSpecializationDecl*> pu = classTemplateSpecializationDecl->getSpecializedTemplateOrPartial();
				if (pu.is<clang::ClassTemplateDecl*>())
				{
					specializedFromDecl = pu.get<clang::ClassTemplateDecl*>();
				}
				else if (pu.is<clang::ClassTemplatePartialSpecializationDecl*>())
				{
					specializedFromDecl = pu.get<clang::ClassTemplatePartialSpecializationDecl*>();
				}

				m_client->onTemplateSpecializationParsed(
					parseLocation,
					declNameHierarchy,
					m_declNameCache->getValue(specializedFromDecl)); // todo: use context and childcontext!!
			}
			else if (clang::FunctionDecl* functionDecl = clang::dyn_cast<clang::FunctionDecl>(d))
			{
				m_client->onTemplateSpecializationParsed(
					parseLocation,
					declNameHierarchy,
					m_declNameCache->getValue(functionDecl->getPrimaryTemplate()->getTemplatedDecl())); // TODO: use context and childcontext!!
			}
			break;
		}
	case RT_TemplateArgument:
		{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onTemplateArgumentTypeParsed(
				parseLocation, declNameHierarchy, contextNameHierarchy);
			break;
		}

	case RT_Called:
		{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onCallParsed(
				parseLocation, contextNameHierarchy, declNameHierarchy);
			break;
		}
	case RT_Reference:
		{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onTypeUsageParsed(
				parseLocation, contextNameHierarchy, declNameHierarchy);
			break;
		}
	case RT_TemplateDefaultArgument:
		{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onTemplateDefaultArgumentTypeParsed(
				parseLocation, declNameHierarchy, contextNameHierarchy);
			break;
		}
	case RT_BaseClass:
		{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onInheritanceParsed(
				parseLocation, contextNameHierarchy, declNameHierarchy, m_contextAccess);
			break;
		}
	case RT_Assigned:
	case RT_Read:
	case RT_Initialized:
	case RT_Modified:
	case RT_Other:
	case RT_AddressTaken:
	{
			const NameHierarchy contextNameHierarchy = getContextName();
			m_client->onUsageParsed(
				parseLocation,
				contextNameHierarchy,
				symbolType,
				declNameHierarchy
			);
			break;
		}
	case RT_Qualifier:
		// Do nothing.
		break;
	default:
		{
			fallback = true;
			break;
		}
	}

	if (fallback)
	{
		std::string name = declNameHierarchy.back()->getName();
		declNameHierarchy.pop();
		declNameHierarchy.push(std::make_shared<NameElement>(name, NameElement::Signature("", "[" + std::to_string(symbolType) + "|" + std::to_string(refType) + "]")));

		m_client->onFunctionParsed(
			parseLocation,
			declNameHierarchy,
			parseLocation,
			declIsImplicit
		);
	}





	//beginLoc = m_indexerContext.sourceManager().getSpellingLoc(beginLoc);
	//clang::FileID fileID;
	//if (beginLoc.isValid())
	//    fileID = m_indexerContext.sourceManager().getFileID(beginLoc);
	//IndexerFileContext &fileContext = m_indexerContext.fileContext(fileID);
	//indexdb::ID symbolID = fileContext.getDeclSymbolID(d);

	//// Pass the prepared data to the IndexBuilder to record.
	//fileContext.builder().recordRef(
	//            symbolID,
	//            range.first,
	//            range.second,
	//            fileContext.getRefTypeID(refType));
	//if (symbolType != ST_Max) {
	//    fileContext.builder().recordSymbol(
	//                symbolID,
	//                fileContext.getSymbolTypeID(symbolType));
	//    if (symbolType != ST_LocalVariable && symbolType != ST_Parameter) {
	//        fileContext.builder().recordGlobalSymbol(symbolID);
	//    }
	//}
}

bool ASTVisitor::isImplicit(clang::Decl* d) const
{
	if (!d)
	{
		return false;
	}

	if (d->isImplicit())
	{
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

bool ASTVisitor::isLocatedInUnparsedProjectFile(clang::SourceLocation loc)
{
	clang::SourceManager& sourceManager = m_context->getSourceManager();
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
		if (m_context->getSourceManager().isWrittenInMainFile(spellingLoc))
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
				ret = m_fileRegister->includeFileIsParsing(filePath.str());
			}
		}
		m_inUnparsedProjectFileMap[fileId] = ret;
		return ret;
	}
	return false;
}

bool ASTVisitor::isLocatedInProjectFile(clang::SourceLocation loc)
{
	clang::SourceManager& sourceManager = m_context->getSourceManager();
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
			bool ret = m_fileRegister->getFileManager()->hasFilePath(filePath.str());
			m_inProjectFileMap[fileId] = ret;
			return ret;
		}
	}
	return false;
}

ParserClient::AccessType ASTVisitor::convertAccessType(clang::AccessSpecifier access) const
{
	switch (access)
	{
	case clang::AS_public:
		return ParserClient::ACCESS_PUBLIC;
	case clang::AS_protected:
		return ParserClient::ACCESS_PROTECTED;
	case clang::AS_private:
		return ParserClient::ACCESS_PRIVATE;
	case clang::AS_none:
		return ParserClient::ACCESS_NONE;
	}
}

ParserClient::AbstractionType ASTVisitor::getAbstractionType(const clang::CXXMethodDecl* methodDecl) const
{
	ParserClient::AbstractionType abstraction = ParserClient::ABSTRACTION_NONE;
	if (methodDecl->isPure())
	{
		abstraction = ParserClient::ABSTRACTION_PURE_VIRTUAL;
	}
	else if (methodDecl->isVirtual())
	{
		abstraction = ParserClient::ABSTRACTION_VIRTUAL;
	}
	return abstraction;
}

ParseLocation ASTVisitor::getParseLocationOfRecordBody(clang::RecordDecl* decl) const
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

ParseLocation ASTVisitor::getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const
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

ParseLocation ASTVisitor::getParseLocation(const clang::SourceRange& sourceRange) const
{
	if (sourceRange.isInvalid())
	{
		return ParseLocation();
	}

	const clang::SourceManager& sourceManager = m_context->getSourceManager();

	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn()
	);
}

NameHierarchy ASTVisitor::getContextName() const
{
	if (m_contextNameGenerator)
	{
		return m_contextNameGenerator->getName();
	}
	return NameHierarchy("global");
}
