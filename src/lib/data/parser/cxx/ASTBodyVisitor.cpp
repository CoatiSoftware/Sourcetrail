#include "data/parser/cxx/ASTBodyVisitor.h"

ASTBodyVisitor::ASTBodyVisitor(ASTBodyVisitorClient* client, clang::NamedDecl* parentDecl)
	: m_client(client)
	, m_parentDecl(parentDecl)
{
}

ASTBodyVisitor::~ASTBodyVisitor()
{
}

void ASTBodyVisitor::VisitStmt(clang::Stmt* stmt)
{
	VisitChildren(stmt);
}

void ASTBodyVisitor::VisitChildren(clang::Stmt* stmt)
{
	for (clang::Stmt::child_range it = stmt->children(); it; it++)
	{
		if (*it)
		{
			static_cast<ASTBodyVisitor*>(this)->Visit(*it);
		}
	}
}

void ASTBodyVisitor::VisitCallExpr(clang::CallExpr* expr)
{
	m_client->VisitCallExprInDeclBody(m_parentDecl, expr);

	VisitStmt(expr);
}

void ASTBodyVisitor::VisitCXXConstructExpr(clang::CXXConstructExpr* expr)
{
	m_client->VisitCXXConstructExprInDeclBody(m_parentDecl, expr);

	VisitStmt(expr);
}
