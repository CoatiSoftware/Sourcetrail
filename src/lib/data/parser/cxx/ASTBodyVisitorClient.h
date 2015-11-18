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

	virtual void VisitCallExprInDeclBody(clang::FunctionDecl* decl, clang::CallExpr* expr) = 0;
	virtual void VisitCallExprInDeclBody(clang::DeclaratorDecl* decl, clang::CallExpr* expr) = 0;
	virtual void VisitCXXConstructExprInDeclBody(clang::FunctionDecl* decl, clang::CXXConstructExpr* expr) = 0;
	virtual void VisitCXXConstructExprInDeclBody(clang::DeclaratorDecl* decl, clang::CXXConstructExpr* expr) = 0;
	virtual void VisitCXXNewExprInDeclBody(clang::FunctionDecl* decl, clang::CXXNewExpr* expr) = 0;
	virtual void VisitCXXNewExprInDeclBody(clang::DeclaratorDecl* decl, clang::CXXNewExpr* expr) = 0;
	virtual void VisitMemberExprInDeclBody(clang::FunctionDecl* decl, clang::MemberExpr* expr) = 0;
	virtual void VisitMemberExprInDeclBody(clang::DeclaratorDecl* decl, clang::MemberExpr* expr) = 0;
	virtual void VisitGlobalVariableExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr) = 0;
	virtual void VisitGlobalVariableExprInDeclBody(clang::DeclaratorDecl* decl, clang::DeclRefExpr* expr) = 0;
	virtual void VisitEnumExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr) = 0;
	virtual void VisitEnumExprInDeclBody(clang::DeclaratorDecl* decl, clang::DeclRefExpr* expr) = 0;
	virtual void VisitVarDeclInDeclBody(clang::FunctionDecl* decl, clang::VarDecl* varDecl) = 0;
};

#endif // AST_BODY_VISITOR_CLIENT_H
