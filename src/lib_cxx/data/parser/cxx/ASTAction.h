#ifndef AST_ACTION_H
#define AST_ACTION_H

#include <memory>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"

#include "data/parser/cxx/ASTConsumer.h"
#include "data/parser/cxx/CommentHandler.h"
#include "utility/file/FileRegister.h"

class ASTAction : public clang::ASTFrontendAction
{
public:
	explicit ASTAction(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister);
	virtual ~ASTAction();

protected:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile);

	virtual bool BeginSourceFileAction(clang::CompilerInstance& compiler, llvm::StringRef filePath);

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
	CommentHandler m_commentHandler;

};

#endif // AST_ACTION_H
