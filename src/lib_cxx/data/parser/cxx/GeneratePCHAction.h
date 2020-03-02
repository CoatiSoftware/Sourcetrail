#ifndef GENERATE_PCH_ACTION_H
#define GENERATE_PCH_ACTION_H

#include <clang/Frontend/FrontendActions.h>

class ParserClient;
class CanonicalFilePathCache;

class GeneratePCHAction: public clang::GeneratePCHAction
{
public:
	explicit GeneratePCHAction(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);

protected:
	// this method has been overridden to prevent erasing output file independently of provided flags
	bool shouldEraseOutputFiles() override;

	// this method has been overridden to always set "AllowASTWithErrors" of the PCHGenerator to "true"
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
		clang::CompilerInstance& CI, llvm::StringRef InFile) override;

	bool BeginSourceFileAction(clang::CompilerInstance& compiler) override;

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;
};

#endif	  // GENERATE_PCH_ACTION_H
