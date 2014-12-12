#include "data/parser/cxx/ASTAction.h"

ASTAction::ASTAction(ParserClient* client)
	: m_client(client)
{
}

ASTAction::~ASTAction()
{
}

std::unique_ptr<clang::ASTConsumer> ASTAction::CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile)
{
	return std::unique_ptr<clang::ASTConsumer>(new ASTConsumer(&compiler.getASTContext(), m_client));
}
