#ifndef AST_ACTION_H
#define AST_ACTION_H

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"

#include "data/parser/cxx/ASTConsumer.h"
#include "utility/file/FileManager.h"

class ASTAction : public clang::ASTFrontendAction
{
public:
	explicit ASTAction(ParserClient* client, FileManager* fileManager);
	virtual ~ASTAction();

	virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, llvm::StringRef inFile);

	virtual bool BeginSourceFileAction(clang::CompilerInstance& compiler, llvm::StringRef filePath);

private:
	ParserClient* m_client;
	FileManager* m_fileManager;
};

#endif // AST_ACTION_H
