#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <memory>

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "data/parser/ParserClient.h"

class ASTVisitor: public clang::RecursiveASTVisitor<ASTVisitor>
{
public:
	ASTVisitor(clang::ASTContext* context, std::shared_ptr<ParserClient> client);
	virtual ~ASTVisitor();

	virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration);

private:
	clang::ASTContext* m_context;
	std::shared_ptr<ParserClient> m_client;
};

#endif // AST_VISITOR_H
