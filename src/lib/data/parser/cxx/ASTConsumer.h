#ifndef AST_CONSUMER_H
#define AST_CONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

#include "data/parser/cxx/AstVisitor.h"

class ASTConsumer : public clang::ASTConsumer
{
public:
	explicit ASTConsumer(clang::ASTContext* context, std::shared_ptr<ParserClient> client);
	virtual ~ASTConsumer();

	virtual void HandleTranslationUnit(clang::ASTContext& context);

private:
	ASTVisitor m_visitor;
};

#endif // AST_CONSUMER_H
