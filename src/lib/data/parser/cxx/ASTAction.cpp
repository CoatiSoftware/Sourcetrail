#include "data/parser/cxx/ASTAction.h"

ASTAction::ASTAction(ParserClient* client)
	: m_client(client)
{
}

ASTAction::~ASTAction()
{
}

clang::ASTConsumer* ASTAction::CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile)
{
	return new ASTConsumer(&compiler.getASTContext(), m_client);
}
