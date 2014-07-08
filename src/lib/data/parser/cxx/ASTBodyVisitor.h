#ifndef AST_BODY_VISITOR
#define AST_BODY_VISITOR

#include "clang/AST/StmtVisitor.h"
#include "data/parser/cxx/ASTBodyVisitorClient.h"

class ASTBodyVisitor: public clang::StmtVisitor<ASTBodyVisitor>
{
public:
	ASTBodyVisitor(ASTBodyVisitorClient* client, clang::NamedDecl* parentDecl);
	virtual ~ASTBodyVisitor();

	void VisitStmt(clang::Stmt* stmt);
	void VisitChildren(clang::Stmt* stmt);
	void VisitCallExpr(clang::CallExpr* expr);
	void VisitCXXConstructExpr(clang::CXXConstructExpr* expr);

private:
	ASTBodyVisitorClient* m_client;
	clang::NamedDecl* m_parentDecl;
};

#endif // AST_BODY_VISITOR
