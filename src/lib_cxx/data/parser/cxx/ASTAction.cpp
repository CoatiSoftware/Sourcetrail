#include "ASTAction.h"

#include <clang/Frontend/CompilerInstance.h>

#include "ASTConsumer.h"
#include "PreprocessorCallbacks.h"

ASTAction::ASTAction(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::shared_ptr<IndexerStateInfo> indexerStateInfo)
	: m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_indexerStateInfo(indexerStateInfo)
	, m_commentHandler(client, canonicalFilePathCache)
{
}

std::unique_ptr<clang::ASTConsumer> ASTAction::CreateASTConsumer(
	clang::CompilerInstance& compiler, llvm::StringRef inFile)
{
	return std::unique_ptr<clang::ASTConsumer>(new ASTConsumer(
		&compiler.getASTContext(),
		&compiler.getPreprocessor(),
		m_client,
		m_canonicalFilePathCache,
		m_indexerStateInfo));
}

bool ASTAction::BeginSourceFileAction(clang::CompilerInstance& compiler)
{
	clang::Preprocessor& preprocessor = compiler.getPreprocessor();
	preprocessor.addPPCallbacks(std::make_unique<PreprocessorCallbacks>(
		compiler.getSourceManager(), m_client, m_canonicalFilePathCache));
	preprocessor.addCommentHandler(&m_commentHandler);
	return true;
}
