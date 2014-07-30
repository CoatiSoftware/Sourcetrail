#ifndef AST_BODY_VISITOR
#define AST_BODY_VISITOR

#include "clang/AST/StmtVisitor.h"
#include "data/parser/cxx/ASTBodyVisitorClient.h"

class ASTBodyVisitor: public clang::StmtVisitor<ASTBodyVisitor>
{
public:
	ASTBodyVisitor(ASTBodyVisitorClient* client, clang::FunctionDecl* functionDecl);
	ASTBodyVisitor(ASTBodyVisitorClient* client, clang::VarDecl* varDecl);
	virtual ~ASTBodyVisitor();

	void VisitStmt(clang::Stmt* stmt);
	void VisitChildren(clang::Stmt* stmt);
	void VisitCallExpr(clang::CallExpr* expr);
	void VisitCXXConstructExpr(clang::CXXConstructExpr* expr);
	void VisitMemberExpr(clang::make_ptr<clang::MemberExpr>::type expr);
	void VisitDeclRefExpr(clang::make_ptr<clang::DeclRefExpr>::type expr);
	void VisitDeclStmt(clang::DeclStmt* stmt);

private:
	ASTBodyVisitorClient* m_client;
	clang::FunctionDecl* m_functionDecl;
	clang::VarDecl* m_varDecl;
};

#endif // AST_BODY_VISITOR
