#ifndef AST_ACTION_H
#define AST_ACTION_H

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"

#include "data/parser/cxx/ASTConsumer.h"
#include "data/parser/cxx/CommentHandler.h"
#include "utility/file/FileRegister.h"

class ASTAction : public clang::ASTFrontendAction
{
public:
	explicit ASTAction(ParserClient* client, FileRegister* fileRegister);
	virtual ~ASTAction();

protected:
	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile);

	virtual bool BeginSourceFileAction(clang::CompilerInstance& compiler, llvm::StringRef filePath);
	virtual void EndSourceFileAction();

private:
	ParserClient* m_client;
	FileRegister* m_fileRegister;
	CommentHandler m_commentHandler;

};

#endif // AST_ACTION_H
