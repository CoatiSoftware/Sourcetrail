#include "data/parser/cxx/ASTAction.h"

ASTAction::ASTAction(ParserClient* client, FileManager* fileManager)
	: m_client(client)
	, m_fileManager(fileManager)
{
}

ASTAction::~ASTAction()
{
}

std::unique_ptr<clang::ASTConsumer> ASTAction::CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile)
{
	return std::unique_ptr<clang::ASTConsumer>(new ASTConsumer(&compiler.getASTContext(), m_client, m_fileManager));
}
