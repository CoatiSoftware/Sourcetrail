#include "data/parser/cxx/CxxAstVisitorComponentContext.h"

CxxAstVisitorComponentContext::CxxAstVisitorComponentContext(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
{
}

CxxAstVisitorComponentContext::~CxxAstVisitorComponentContext()
{
}

const clang::NamedDecl* CxxAstVisitorComponentContext::getTopmostContextDecl() const
{
	for (std::vector<std::shared_ptr<CxxContext>>::const_reverse_iterator it = m_contextStack.rbegin(); it != m_contextStack.rend(); it ++)
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

NameHierarchy CxxAstVisitorComponentContext::getContextName(const size_t skip)
{
	size_t skipped = 0;

	for (auto it = m_contextStack.rbegin(); it != m_contextStack.rend(); it++)
	{
		if (*it)
		{
			if (skipped >= skip)
			{
				return (*it)->getName();
			}
			else
			{
				skipped++;
			}
		}
	}
	return getAstVisitor()->getDeclNameCache()->getValue(nullptr);
}

NameHierarchy CxxAstVisitorComponentContext::getContextName(const NameHierarchy& fallback, const size_t skip)
{
	size_t skipped = 0;

	for (auto it = m_contextStack.rbegin(); it != m_contextStack.rend(); it++)
	{
		if (*it)
		{
			if (skipped >= skip)
			{
				return (*it)->getName();
			}
			else
			{
				skipped++;
			}
		}
	}
	return fallback;
}

void CxxAstVisitorComponentContext::beginTraverseDecl(clang::Decl* d)
{
	std::shared_ptr<CxxContextDecl> context;

	if (d &&
		clang::isa<clang::NamedDecl>(d) &&
		!clang::isa<clang::ParmVarDecl>(d) &&											// no parameter
		!(clang::isa<clang::VarDecl>(d) && d->getParentFunctionOrMethod() != NULL) &&	// no local variable
		!clang::isa<clang::UsingDirectiveDecl>(d) &&									// no using directive decl
		!clang::isa<clang::UsingDecl>(d) &&												// no using decl
		!clang::isa<clang::NamespaceDecl>(d)											// no namespace
	){
		clang::NamedDecl* nd = clang::dyn_cast<clang::NamedDecl>(d);
		context = std::make_shared<CxxContextDecl>(nd, getAstVisitor()->getDeclNameCache());
	}

	m_contextStack.push_back(context);
}

void CxxAstVisitorComponentContext::endTraverseDecl(clang::Decl* d)
{
	m_contextStack.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseTypeLoc(const clang::TypeLoc& tl)
{
	std::shared_ptr<CxxContextType> context;

	if (!getAstVisitor()->checkIgnoresTypeLoc(tl))
	{
		context = std::make_shared<CxxContextType>(tl.getTypePtr(), getAstVisitor()->getTypeNameCache());
	}

	m_contextStack.push_back(context);
}

void CxxAstVisitorComponentContext::endTraverseTypeLoc(const clang::TypeLoc& tl)
{
	m_contextStack.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseLambdaExpr(clang::LambdaExpr* s)
{
	clang::CXXMethodDecl* methodDecl = s->getCallOperator();
	m_contextStack.push_back(std::make_shared<CxxContextDecl>(methodDecl, getAstVisitor()->getDeclNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseLambdaExpr(clang::LambdaExpr* s)
{
	m_contextStack.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseFunctionDecl(clang::FunctionDecl* d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d, getAstVisitor()->getDeclNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseFunctionDecl(clang::FunctionDecl* d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d, getAstVisitor()->getDeclNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(d, getAstVisitor()->getDeclNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseDeclRefExpr(clang::DeclRefExpr* s)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextDecl>(s->getDecl(), getAstVisitor()->getDeclNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseDeclRefExpr(clang::DeclRefExpr* s)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc)
{
	m_templateArgumentContext.push_back(std::make_shared<CxxContextType>(loc.getTypePtr(), getAstVisitor()->getTypeNameCache()));
}

void CxxAstVisitorComponentContext::endTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc)
{
	m_templateArgumentContext.pop_back();
}

void CxxAstVisitorComponentContext::beginTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) // TODO: do this for unresolved and dependent stuff
{
	std::shared_ptr<CxxContext> clear;
	m_templateArgumentContext.push_back(clear);
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
