#ifndef AST_ACTION_H
#define AST_ACTION_H

#include <memory>

#include <clang/Frontend/FrontendAction.h>

#include "CommentHandler.h"

class ParserClient;
class CanonicalFilePathCache;
struct IndexerStateInfo;

class ASTAction: public clang::ASTFrontendAction
{
public:
	explicit ASTAction(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::shared_ptr<IndexerStateInfo> indexerStateInfo);

protected:
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
		clang::CompilerInstance& compiler, llvm::StringRef inFile) override;
	bool BeginSourceFileAction(clang::CompilerInstance& compiler) override;

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
	CommentHandler m_commentHandler;
};

#endif	  // AST_ACTION_H
