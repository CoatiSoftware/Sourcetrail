#ifndef AST_BODY_VISITOR_CLIENT_H
#define AST_BODY_VISITOR_CLIENT_H

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"

class ASTBodyVisitorClient
{
public:
	ASTBodyVisitorClient();
	virtual ~ASTBodyVisitorClient();

	virtual void VisitCallExprInDeclBody(clang::NamedDecl* decl, clang::CallExpr* expr) = 0;
	virtual void VisitCXXConstructExprInDeclBody(clang::NamedDecl* decl, clang::CXXConstructExpr* expr) = 0;
	virtual void VisitFieldUsageExprInDeclBody(clang::NamedDecl* decl, clang::MemberExpr* expr) = 0;
	virtual void VisitGlobalVariableUsageExprInDeclBody(clang::NamedDecl* decl, clang::DeclRefExpr* expr) = 0;
};

#endif // AST_BODY_VISITOR_CLIENT_H
