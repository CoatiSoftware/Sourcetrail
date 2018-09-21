#ifndef AST_ACTION_H
#define AST_ACTION_H

#include <memory>

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Lex/Preprocessor.h>

#include "ASTConsumer.h"
#include "CommentHandler.h"
#include "PreprocessorCallbacks.h"

template <typename ASTActionBase>
class ASTAction
	: public ASTActionBase
{
public:
	explicit ASTAction(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::shared_ptr<IndexerStateInfo> indexerStateInfo
	)
		: m_client(client)
		, m_canonicalFilePathCache(canonicalFilePathCache)
		, m_indexerStateInfo(indexerStateInfo)
		, m_commentHandler(client, canonicalFilePathCache)
	{}

	virtual ~ASTAction() {}

protected:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile) override
	{
		return std::unique_ptr<clang::ASTConsumer>(
			new ASTConsumer(&compiler.getASTContext(), &compiler.getPreprocessor(), m_client, m_canonicalFilePathCache, m_indexerStateInfo));
	}

	virtual bool BeginSourceFileAction(clang::CompilerInstance& compiler) override
	{
		clang::Preprocessor& preprocessor = compiler.getPreprocessor();
		preprocessor.addPPCallbacks(
			llvm::make_unique<PreprocessorCallbacks>(compiler.getSourceManager(), m_client, m_canonicalFilePathCache));
		preprocessor.addCommentHandler(&m_commentHandler);
		return true;
	}

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
	CommentHandler m_commentHandler;
};

#endif // AST_ACTION_H
