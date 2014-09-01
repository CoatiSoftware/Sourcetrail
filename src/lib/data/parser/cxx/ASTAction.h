#ifndef AST_ACTION_H
#define AST_ACTION_H

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"

#include "data/parser/cxx/ASTConsumer.h"

class ASTAction : public clang::ASTFrontendAction
{
public:
	explicit ASTAction(ParserClient* client);
	virtual ~ASTAction();

	virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile);

private:
	ParserClient* m_client;
};

#endif // AST_ACTION_H
