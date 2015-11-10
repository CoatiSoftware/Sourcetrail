#include "data/parser/cxx/ASTAction.h"

#include "clang/Lex/Preprocessor.h"

#include "data/parser/cxx/CommentHandler.h"
#include "data/parser/cxx/PreprocessorCallbacks.h"

ASTAction::ASTAction(ParserClient* client, FileRegister* fileRegister)
	: m_client(client)
	, m_fileRegister(fileRegister)
	, m_commentHandler(client, fileRegister)
{
}

ASTAction::~ASTAction()
{
}

std::unique_ptr<clang::ASTConsumer> ASTAction::CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile)
{
	return std::unique_ptr<clang::ASTConsumer>(new ASTConsumer(&compiler.getASTContext(), m_client, m_fileRegister));
}

bool ASTAction::BeginSourceFileAction(clang::CompilerInstance& compiler, llvm::StringRef filePath)
{
	clang::Preprocessor& preprocessor = compiler.getPreprocessor();
	preprocessor.addPPCallbacks(
		llvm::make_unique<PreprocessorCallbacks>(compiler.getSourceManager(), m_client, m_fileRegister));
	preprocessor.addCommentHandler(&m_commentHandler);
	return true;
}

void ASTAction::EndSourceFileAction()
{
	m_fileRegister->markParsingIncludeFilesParsed();
}
