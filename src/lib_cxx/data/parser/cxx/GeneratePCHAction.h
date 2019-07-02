#ifndef GENERATE_PCH_ACTION_H
#define GENERATE_PCH_ACTION_H

#include "clang/Frontend/FrontendActions.h"

class GeneratePCHAction : public clang::GeneratePCHAction
{
protected:
	// this method has been overridden to prevent erasing output file independently of provided flags
	bool shouldEraseOutputFiles() override;

	// this method has been overridden to always set "AllowASTWithErrors" of the PCHGenerator to "true"
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef InFile) override;
};

#endif // GENERATE_PCH_ACTION_H
