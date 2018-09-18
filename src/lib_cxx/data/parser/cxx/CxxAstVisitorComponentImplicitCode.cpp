#include "CxxAstVisitorComponentImplicitCode.h"

CxxAstVisitorComponentImplicitCode::CxxAstVisitorComponentImplicitCode(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
{
}

bool CxxAstVisitorComponentImplicitCode::shouldVisitImplicitCode() const
{
	if (!m_stack.empty())
	{
		return m_stack.back();
	}
	return true;
}

void CxxAstVisitorComponentImplicitCode::beginTraverseDecl(clang::Decl* d)
{
	m_stack.push_back(true);
}

void CxxAstVisitorComponentImplicitCode::endTraverseDecl(clang::Decl* d)
{
	m_stack.pop_back();
}

void CxxAstVisitorComponentImplicitCode::beginTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s)
{
	m_stack.push_back(false);
}

void CxxAstVisitorComponentImplicitCode::endTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s)
{
	m_stack.pop_back();
}
