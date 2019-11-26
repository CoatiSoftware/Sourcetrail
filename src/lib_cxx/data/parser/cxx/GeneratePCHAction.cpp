#include "GeneratePCHAction.h"

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/MultiplexConsumer.h>
#include <clang/Serialization/ASTWriter.h>
#include <clang/Lex/PreprocessorOptions.h>

#include "PreprocessorCallbacks.h"

GeneratePCHAction::GeneratePCHAction(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache)
	: m_client(client), m_canonicalFilePathCache(canonicalFilePathCache)
{
}

bool GeneratePCHAction::shouldEraseOutputFiles()
{
	return false;
}

std::unique_ptr<clang::ASTConsumer> GeneratePCHAction::CreateASTConsumer(
	clang::CompilerInstance& CI, llvm::StringRef InFile)
{
	std::string Sysroot;
	if (!ComputeASTConsumerArguments(CI, /*ref*/ Sysroot))
		return nullptr;

	std::string OutputFile;
	std::unique_ptr<llvm::raw_pwrite_stream> OS = CreateOutputFile(CI, InFile, /*ref*/ OutputFile);
	if (!OS)
		return nullptr;

	if (!CI.getFrontendOpts().RelocatablePCH)
		Sysroot.clear();

	const auto& FrontendOpts = CI.getFrontendOpts();
	auto Buffer = std::make_shared<clang::PCHBuffer>();
	std::vector<std::unique_ptr<clang::ASTConsumer>> Consumers;
	Consumers.push_back(std::make_unique<clang::PCHGenerator>(
		CI.getPreprocessor(),
		CI.getModuleCache(),
		OutputFile,
		Sysroot,
		Buffer,
		FrontendOpts.ModuleFileExtensions,
		true, // always allow errors in the PCH
		FrontendOpts.IncludeTimestamps,
		+CI.getLangOpts().CacheGeneratedPCH));
	Consumers.push_back(CI.getPCHContainerWriter().CreatePCHContainerGenerator(
		CI, InFile, OutputFile, std::move(OS), Buffer));

	return llvm::make_unique<clang::MultiplexConsumer>(std::move(Consumers));
}

bool GeneratePCHAction::BeginSourceFileAction(clang::CompilerInstance& compiler)
{
	clang::Preprocessor& preprocessor = compiler.getPreprocessor();
	preprocessor.addPPCallbacks(llvm::make_unique<PreprocessorCallbacks>(
		compiler.getSourceManager(), m_client, m_canonicalFilePathCache));
	return true;
}
