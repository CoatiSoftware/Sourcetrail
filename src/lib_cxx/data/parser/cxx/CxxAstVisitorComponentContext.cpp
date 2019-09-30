#include "CxxAstVisitorComponentContext.h"

#include "data/parser/cxx/CxxAstVisitor.h"

CxxAstVisitorComponentContext::CxxAstVisitorComponentContext(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
{
}

const clang::NamedDecl* CxxAstVisitorComponentContext::getTopmostContextDecl() const
{
	for (auto it = m_contextStack.rbegin(); it != m_contextStack.rend(); it++)
	{
		if (*it)
		{
			const clang::NamedDecl* decl = (*it)->getDecl();
			if (decl)
			{
				return decl;
			}
		}
	}
	return nullptr;
}

const CxxContext* CxxAstVisitorComponentContext::getContext(const size_t skip)
{
	size_t skipped = 0;

	for (auto it = m_contextStack.rbegin(); it != m_contextStack.rend(); it++)
	{
		if (*it)
		{
			if (skipped >= skip)
			{
				return it->get();
			}
			else
			{
				skipped++;
			}
		}
	}
	return nullptr;
}

void CxxAstVisitorComponentContext::beginTraverseDecl(clang::Decl* d)
{
	std::shared_ptr<CxxContextDecl> context;

	if (d &&
		clang::isa<clang::NamedDecl>(d) &&
		!clang::isa<clang::ParmVarDecl>(d) &&												// no parameter
		!(clang::isa<clang::VarDecl>(d) && d->getParentFunctionOrMethod() != nullptr) &&	// no local variable
		!clang::isa<clang::UsingDirectiveDecl>(d) &&										// no using directive decl
		!clang::isa<clang::UsingDecl>(d) &&													// no using decl
		!clang::isa<clang::NamespaceDecl>(d) &&												// no namespace
		!clang::isa<clang::NonTypeTemplateParmDecl>(d) &&									// no template params
		!clang::isa<clang::TemplateTypeParmDecl>(d) &&										// no template params
		!clang::isa<clang::TemplateTemplateParmDecl>(d) 									// no template params
	){
		clang::NamedDecl* nd = clang::dyn_cast<clang::NamedDecl>(d);
		context = std::make_shared<CxxContextDecl>(nd);
	}

	m_contextStack.push_back(context);
}

void CxxAstVisitorComponentContext::endTraverseDecl(clang::Decl* d)
{
	m_contextStack.pop_back();
}
#include "CxxTypeNameResolver.h"
void CxxAstVisitorComponentContext::beginTraverseTypeLoc(const clang::TypeLoc& tl)
{
	std::wstring namesss;
	std::unique_ptr<CxxTypeName> typeName =
		CxxTypeNameResolver(getAstVisitor()->getCanonicalFilePathCache()).getName(tl.getTypePtr());
	if (typeName)
	{
		namesss = typeName->toNameHierarchy().getQualifiedNameWithSignature();
	}





	std::shared_ptr<CxxContextType> context;
	clang::TypeLoc::TypeLocClass tlcc = tl.getTypeLocClass();
	if (!getAstVisitor()->checkIgnoresTypeLoc(tl))
	{
		bool recordContext = true;
		if (tl.getTypeLocClass() == clang::TypeLoc::TemplateSpecialization)
		{
			const clang::TemplateSpecializationTypeLoc& tstl = tl.castAs<clang::TemplateSpecializationTypeLoc>();
			const clang::TemplateSpecializationType* tst = tstl.getTypePtr();
			if (tst && tst->getTemplateName().isDependent())
			{
				recordContext = false;
			}
		}
		if (recordContext)
		{
			context = std::make_shared<CxxContextType>(tl.getTypePtr());
		}
	}

	m_contextStack.push_back(context);
}

void CxxAstVisitorComponentContext::endTraverseTypeLoc(const clang::TypeLoc& tl)
{
	m_contextStack.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseLambdaExpr(clang::LambdaExpr* s)
{
	m_contextStack.push_back(std::make_shared<CxxContextDecl>(s->getCallOperator()));
}

void CxxAstVisitorComponentContext::endTraverseLambdaExpr(clang::LambdaExpr* s)
{
	m_contextStack.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseFunctionDecl(clang::FunctionDecl* d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d));
}

void CxxAstVisitorComponentContext::endTraverseFunctionDecl(clang::FunctionDecl* d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d));
}

void CxxAstVisitorComponentContext::endTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d));
}

void CxxAstVisitorComponentContext::endTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseDeclRefExpr(clang::DeclRefExpr* s)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(s->getDecl()));
}

void CxxAstVisitorComponentContext::endTraverseDeclRefExpr(clang::DeclRefExpr* s)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc)
{
	std::wstring namesss;
	std::unique_ptr<CxxTypeName> typeName =
		CxxTypeNameResolver(getAstVisitor()->getCanonicalFilePathCache()).getName(loc.getTypePtr());
	if (typeName)
	{
		namesss = typeName->toNameHierarchy().getQualifiedNameWithSignature();
	}

	bool recordContext = true;
	const clang::TemplateSpecializationType* tst = loc.getTypePtr();
	if (tst && tst->getTemplateName().isDependent())
	{
		recordContext = false;
	}

	if (recordContext)
	{
		m_templateArgumentContext.push_back(std::make_shared<CxxContextType>(loc.getTypePtr()));
	}
	else
	{
		m_templateArgumentContext.push_back(std::shared_ptr<CxxContextType>());
	}
}

void CxxAstVisitorComponentContext::endTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) // TODO: do this for unresolved and dependent stuff
{
	m_templateArgumentContext.push_back(nullptr);
}

void CxxAstVisitorComponentContext::endTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	std::shared_ptr<CxxContext> context;

	if (!m_templateArgumentContext.empty())
	{
		context = m_templateArgumentContext.back();
	}

	m_contextStack.push_back(context);
}

void CxxAstVisitorComponentContext::endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	m_contextStack.pop_back();
}
