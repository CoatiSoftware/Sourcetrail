#include "data/parser/cxx/CxxParser.h"

#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"

#include "data/parser/cxx/ASTActionFactory.h"
#include "data/parser/cxx/CxxDiagnosticConsumer.h"

namespace
{
	static std::vector<std::string> getSyntaxOnlyToolArgs(const std::vector<std::string> &ExtraArgs, llvm::StringRef FileName)
	{
		std::vector<std::string> Args;
		Args.push_back("clang-tool");
		Args.push_back("-fsyntax-only");
		Args.insert(Args.end(), ExtraArgs.begin(), ExtraArgs.end());
		Args.push_back(FileName.str());
		return Args;
	}

	// custom implementation of clang::runToolOnCodeWithArgs which also sets our custon DiagnosticConsumer
	static bool runToolOnCodeWithArgs(
		clang::DiagnosticConsumer* DiagConsumer,
		clang::FrontendAction *ToolAction,
		const llvm::Twine &Code,
		const std::vector<std::string> &Args,
		const llvm::Twine &FileName = "input.cc",
		const clang::tooling::FileContentMappings &VirtualMappedFiles = clang::tooling::FileContentMappings()
	)
	{
		llvm::SmallString<16> FileNameStorage;
		llvm::StringRef FileNameRef = FileName.toNullTerminatedStringRef(FileNameStorage);
		llvm::IntrusiveRefCntPtr<clang::FileManager> Files(new clang::FileManager(clang::FileSystemOptions()));
		clang::tooling::ToolInvocation Invocation(getSyntaxOnlyToolArgs(Args, FileNameRef), ToolAction, Files.get());

		llvm::SmallString<1024> CodeStorage;
		Invocation.mapVirtualFile(FileNameRef,
		Code.toNullTerminatedStringRef(CodeStorage));

		for (auto &FilenameWithContent : VirtualMappedFiles)
		{
			Invocation.mapVirtualFile(FilenameWithContent.first,
			FilenameWithContent.second);
		}

		Invocation.setDiagnosticConsumer(DiagConsumer);

		return Invocation.run();
	}
}

CxxParser::CxxParser(ParserClient* client, FileManager* fileManager)
	: Parser(client)
	, m_fileManager(fileManager)
{
}

CxxParser::~CxxParser()
{
}

void CxxParser::parseFiles(
	const std::vector<std::string>& filePaths,
	const std::vector<std::string>& systemHeaderSearchPaths,
	const std::vector<std::string>& headerSearchPaths
){
	// Commandline flags passed to the programm. Everything after '--' will be interpreted by the ClangTool.
	std::vector<std::string> args;
	args.push_back("app");
	args.push_back("--");

	// verbose
	// args.push_back("-v");

	// The option -fno-delayed-template-parsing signals that templates that there should
	// be AST elements for unused template functions as well.
	args.push_back("-fno-delayed-template-parsing");

	// The option -c signals that no executable is built.
	args.push_back("-c");

	// The option '-x c++' treats subsequent input files as C++.
	args.push_back("-x");
	args.push_back("c++");

	args.push_back("-std=c++11");

	for (const std::string& path : systemHeaderSearchPaths)
	{
		args.push_back("-isystem" + path);
	}

	for (const std::string& path : headerSearchPaths)
	{
		args.push_back("-I" + path);
	}

	int argc = args.size();
	const char** argv = new const char*[argc];
	for (size_t i = 0; i < args.size(); i++)
	{
		argv[i] = args[i].c_str();
	}

	std::shared_ptr<clang::tooling::FixedCompilationDatabase> compilationDatabase(
		clang::tooling::FixedCompilationDatabase::loadFromCommandLine(argc, argv)
	);

	if (!compilationDatabase)
	{
		LOG_ERROR("Failed to load compilation database");
		return;
	}

	clang::tooling::ClangTool tool(*compilationDatabase, filePaths);

	llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> options = new clang::DiagnosticOptions();
	CxxDiagnosticConsumer reporter(llvm::errs(), &*options, m_client);
	tool.setDiagnosticConsumer(&reporter);

	ASTActionFactory actionFactory(m_client, m_fileManager);
	tool.run(&actionFactory);
}

void CxxParser::parseFile(std::shared_ptr<TextAccess> textAccess)
{
	std::vector<std::string> args;
	args.push_back("-fno-delayed-template-parsing");

	llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> options = new clang::DiagnosticOptions();
	CxxDiagnosticConsumer reporter(llvm::errs(), &*options, m_client, false);

	ASTActionFactory actionFactory(m_client, m_fileManager);
	runToolOnCodeWithArgs(&reporter, actionFactory.create(), textAccess->getText(), args);
}
