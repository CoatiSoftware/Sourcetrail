#ifndef AST_ACTION_H
#define AST_ACTION_H

#include <memory>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"

#include "data/parser/cxx/ASTConsumer.h"
#include "data/parser/cxx/cxxCacheTypes.h"
#include "data/parser/cxx/CommentHandler.h"
#include "data/parser/cxx/PreprocessorCallbacks.h"
#include "utility/file/FileRegister.h"

template <typename ASTActionBase>
class ASTAction
	: public ASTActionBase
{
public:
	explicit ASTAction(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<FilePathCache> canonicalFilePathCache
	)
		: m_client(client)
		, m_fileRegister(fileRegister)
		, m_canonicalFilePathCache(canonicalFilePathCache)
		, m_commentHandler(client, fileRegister, canonicalFilePathCache)
	{}

	virtual ~ASTAction() {}

protected:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile)
	{
		return std::unique_ptr<clang::ASTConsumer>(
			new ASTConsumer(&compiler.getASTContext(), &compiler.getPreprocessor(), m_client, m_fileRegister, m_canonicalFilePathCache));
	}

	virtual bool BeginSourceFileAction(clang::CompilerInstance& compiler, llvm::StringRef filePath)
	{
		clang::Preprocessor& preprocessor = compiler.getPreprocessor();
		preprocessor.addPPCallbacks(
			llvm::make_unique<PreprocessorCallbacks>(compiler.getSourceManager(), m_client, m_fileRegister, m_canonicalFilePathCache));
		preprocessor.addCommentHandler(&m_commentHandler);
		return true;
	}

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<FilePathCache> m_canonicalFilePathCache;
	CommentHandler m_commentHandler;
};

#endif // AST_ACTION_H
