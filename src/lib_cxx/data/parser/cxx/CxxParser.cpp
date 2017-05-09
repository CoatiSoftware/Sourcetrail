#include "data/parser/cxx/CxxParser.h"

#include "clang/Tooling/Tooling.h"

#include "utility/file/FilePath.h"
#include "utility/file/FileRegister.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/tracing.h"

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/name/NameHierarchy.h"
#include "data/parser/cxx/ASTActionFactory.h"
#include "data/parser/cxx/CxxCompilationDatabaseSingle.h"
#include "data/parser/cxx/CxxDiagnosticConsumer.h"
#include "settings/LanguageType.h"

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
		Invocation.mapVirtualFile(FileNameRef, Code.toNullTerminatedStringRef(CodeStorage));

		for (auto &FilenameWithContent : VirtualMappedFiles)
		{
			Invocation.mapVirtualFile(FilenameWithContent.first, FilenameWithContent.second);
		}

		Invocation.setDiagnosticConsumer(DiagConsumer);

		return Invocation.run();
	}
}

CxxParser::CxxParser(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister)
	: Parser(client)
	, m_fileRegister(fileRegister)
{
}

CxxParser::~CxxParser()
{
}

void CxxParser::buildIndex(std::shared_ptr<IndexerCommandCxxCdb> indexerCommand)
{
	NameHierarchy::setDelimiter(getSymbolNameDelimiterForLanguage(LANGUAGE_CPP));

	clang::tooling::CompileCommand compileCommand;
	compileCommand.Filename = indexerCommand->getSourceFilePath().str();
	compileCommand.Directory = indexerCommand->getWorkingDirectory().str();
	compileCommand.CommandLine = indexerCommand->getCompilerFlags();

	{
		std::vector<std::string> args = getCommandlineArgumentsEssential(
			std::vector<std::string>(), indexerCommand->getSystemHeaderSearchPaths(), indexerCommand->getFrameworkSearchPaths()
		);
		compileCommand.CommandLine.insert(compileCommand.CommandLine.end(), args.begin(), args.end());
	}

	CxxCompilationDatabaseSingle compilationDatabase(compileCommand);
	clang::tooling::ClangTool tool(compilationDatabase, std::vector<std::string>(1, indexerCommand->getSourceFilePath().str()));

	std::shared_ptr<FilePathCache> canonicalFilePathCache = std::make_shared<FilePathCache>([](std::string fileName) -> FilePath
		{
			return FilePath(fileName).canonical();
		}
	);

	std::shared_ptr<CxxDiagnosticConsumer> diagnostics = getDiagnostics(canonicalFilePathCache, true);
	tool.setDiagnosticConsumer(diagnostics.get());

	ASTActionFactory actionFactory(m_client, m_fileRegister, canonicalFilePathCache, indexerCommand->preprocessorOnly());
	tool.run(&actionFactory);
}

void CxxParser::buildIndex(std::shared_ptr<IndexerCommandCxxManual> indexerCommand)
{
	NameHierarchy::setDelimiter(getSymbolNameDelimiterForLanguage(LANGUAGE_CPP));

	std::shared_ptr<clang::tooling::CompilationDatabase> compilationDatabase = getCompilationDatabase(indexerCommand);

	clang::tooling::ClangTool tool(*compilationDatabase, std::vector<std::string>(1, indexerCommand->getSourceFilePath().str()));

	std::shared_ptr<FilePathCache> canonicalFilePathCache = std::make_shared<FilePathCache>([](std::string fileName) -> FilePath
		{
			return FilePath(fileName).canonical();
		}
	);

	std::shared_ptr<CxxDiagnosticConsumer> diagnostics = getDiagnostics(canonicalFilePathCache, true);
	tool.setDiagnosticConsumer(diagnostics.get());

	ASTActionFactory actionFactory(m_client, m_fileRegister, canonicalFilePathCache, indexerCommand->preprocessorOnly());
	tool.run(&actionFactory);
}

void CxxParser::buildIndex(const std::string& fileName, std::shared_ptr<TextAccess> fileContent)
{
	NameHierarchy::setDelimiter(getSymbolNameDelimiterForLanguage(LANGUAGE_CPP));

	std::shared_ptr<FilePathCache> canonicalFilePathCache = std::make_shared<FilePathCache>([](std::string fileName) -> FilePath
		{
			return FilePath(fileName).canonical();
		}
	);

	std::shared_ptr<CxxDiagnosticConsumer> diagnostics = getDiagnostics(canonicalFilePathCache, false);
	ASTActionFactory actionFactory(m_client, m_fileRegister, canonicalFilePathCache, false);

	std::vector<std::string> args = getCommandlineArgumentsEssential(std::vector<std::string>(1, "-std=c++1z"), std::vector<FilePath>(), std::vector<FilePath>());

	runToolOnCodeWithArgs(
		diagnostics.get(),
		actionFactory.create(),
		fileContent->getText(),
		args,
		fileName
	);
}

std::vector<std::string> CxxParser::getCommandlineArgumentsEssential(
	const std::vector<std::string>& compilerFlags, const std::vector<FilePath>& systemHeaderSearchPaths, const std::vector<FilePath>& frameworkSearchPaths
) const {
	std::vector<std::string> args;

	// verbose
	// args.push_back("-v");

	// The option -fno-delayed-template-parsing signals that templates that there should
	// be AST elements for unused template functions as well.
	args.push_back("-fno-delayed-template-parsing");

	// The option -fexceptions signals that clang should watch out for exception-related code during indexing.
	args.push_back("-fexceptions");

	// The option -c signals that no executable is built.
	args.push_back("-c");

	// The option -w disables all warnings.
	args.push_back("-w");

	// This option tells clang just to continue parsing no matter how manny errors have been thrown.
	args.push_back("-ferror-limit=0");

	args.insert(args.end(), compilerFlags.begin(), compilerFlags.end());

	for (const FilePath& path: systemHeaderSearchPaths)
	{
		args.push_back("-isystem");
		args.push_back(path.str());
	}

	for (const FilePath& path: frameworkSearchPaths)
	{
		args.push_back("-iframework");
		args.push_back(path.str());
	}

	return args;
}

std::vector<std::string> CxxParser::getCommandlineArguments(std::shared_ptr<IndexerCommandCxxManual> indexerCommand) const
{
	std::vector<std::string> args = getCommandlineArgumentsEssential(
		indexerCommand->getCompilerFlags(), indexerCommand->getSystemHeaderSearchPaths(), indexerCommand->getFrameworkSearchPaths()
	);

	// Set language standard
	std::string standard = "-std=" + indexerCommand->getLanguageStandard();
	args.push_back(standard);

	return args;
}

std::shared_ptr<clang::tooling::FixedCompilationDatabase> CxxParser::getCompilationDatabase(
	std::shared_ptr<IndexerCommandCxxManual> indexerCommand
) const {
	// Commandline flags passed to the programm. Everything after '--' will be interpreted by the ClangTool.
	std::vector<std::string> args = getCommandlineArguments(indexerCommand);
	args.insert(args.begin(), "app");
	args.insert(args.begin() + 1, "--");

	int argc = args.size();
	const char** argv = new const char*[argc];
	for (size_t i = 0; i < args.size(); i++)
	{
		argv[i] = args[i].c_str();
	}

	std::shared_ptr<clang::tooling::FixedCompilationDatabase> compilationDatabase(
		clang::tooling::FixedCompilationDatabase::loadFromCommandLine(argc, argv)
	);

	delete[] argv;

	if (!compilationDatabase)
	{
		LOG_ERROR("Failed to load compilation database");
		return nullptr;
	}

	return compilationDatabase;
}

std::shared_ptr<CxxDiagnosticConsumer> CxxParser::getDiagnostics(std::shared_ptr<FilePathCache> canonicalFilePathCache, bool logErrors) const
{
	llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> options = new clang::DiagnosticOptions();
	return std::make_shared<CxxDiagnosticConsumer>(
		llvm::errs(), &*options, m_client, m_fileRegister, canonicalFilePathCache, logErrors);
}
