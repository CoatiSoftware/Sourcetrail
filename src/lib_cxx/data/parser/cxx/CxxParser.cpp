#include "data/parser/cxx/CxxParser.h"

#include "clang/Tooling/Tooling.h"
#include "llvm/Support/TargetSelect.h"

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/parser/cxx/ASTActionFactory.h"
#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/CxxCompilationDatabaseSingle.h"
#include "data/parser/cxx/CxxDiagnosticConsumer.h"

#include "utility/file/FilePath.h"
#include "utility/file/FileRegister.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"
#include "utility/utility.h"

namespace
{
	std::vector<std::string> prependSyntaxOnlyToolArgs(const std::vector<std::string>& args)
	{
		return utility::concat({ "clang-tool", "-fsyntax-only" }, args);
	}

	std::vector<std::string> appendFilePath(const std::vector<std::string>& args, llvm::StringRef fileName)
	{
		return utility::concat(args, { fileName.str() });
	}

	// custom implementation of clang::runToolOnCodeWithArgs which also sets our custon DiagnosticConsumer
	bool runToolOnCodeWithArgs(
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
		clang::tooling::ToolInvocation Invocation(prependSyntaxOnlyToolArgs(appendFilePath(Args, FileNameRef)), ToolAction, Files.get());

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
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmParser();
}

CxxParser::~CxxParser()
{
}

void CxxParser::buildIndex(std::shared_ptr<IndexerCommandCxxCdb> indexerCommand)
{
	clang::tooling::CompileCommand compileCommand;
	compileCommand.Filename = indexerCommand->getSourceFilePath().str();
	compileCommand.Directory = indexerCommand->getWorkingDirectory().str();
	compileCommand.CommandLine = utility::concat(indexerCommand->getCompilerFlags(), getCommandlineArgumentsEssential(
		std::vector<std::string>(), indexerCommand->getSystemHeaderSearchPaths(), indexerCommand->getFrameworkSearchPaths()
	));

	if (!utility::isPrefix("-", compileCommand.CommandLine.front()))
	{
		compileCommand.CommandLine.erase(compileCommand.CommandLine.begin());
	}
	compileCommand.CommandLine = prependSyntaxOnlyToolArgs(compileCommand.CommandLine);

	CxxCompilationDatabaseSingle compilationDatabase(compileCommand);
	runTool(&compilationDatabase, indexerCommand->getSourceFilePath());
}

void CxxParser::buildIndex(std::shared_ptr<IndexerCommandCxxEmpty> indexerCommand)
{
	clang::tooling::CompileCommand compileCommand;
	compileCommand.Filename = indexerCommand->getSourceFilePath().str();
	compileCommand.Directory = indexerCommand->getWorkingDirectory().str();
	compileCommand.CommandLine = prependSyntaxOnlyToolArgs(appendFilePath(getCommandlineArguments(indexerCommand), indexerCommand->getSourceFilePath().str()));

	CxxCompilationDatabaseSingle compilationDatabase(compileCommand);
	runTool(&compilationDatabase, indexerCommand->getSourceFilePath());
}

void CxxParser::buildIndex(const std::string& fileName, std::shared_ptr<TextAccess> fileContent, std::vector<std::string> compilerFlags)
{
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache = std::make_shared<CanonicalFilePathCache>();

	std::shared_ptr<CxxDiagnosticConsumer> diagnostics = getDiagnostics(canonicalFilePathCache, false);
	ASTActionFactory actionFactory(m_client, m_fileRegister, canonicalFilePathCache);

	std::vector<std::string> args = getCommandlineArgumentsEssential(compilerFlags, std::vector<FilePath>(), std::vector<FilePath>());

	runToolOnCodeWithArgs(
		diagnostics.get(),
		actionFactory.create(),
		fileContent->getText(),
		args,
		fileName
	);
}

void CxxParser::runTool(clang::tooling::CompilationDatabase* compilationDatabase, const FilePath& sourceFilePath)
{
	clang::tooling::ClangTool tool(*compilationDatabase, std::vector<std::string>(1, sourceFilePath.str()));

	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache = std::make_shared<CanonicalFilePathCache>();

	std::shared_ptr<CxxDiagnosticConsumer> diagnostics = getDiagnostics(canonicalFilePathCache, true);

	tool.setDiagnosticConsumer(diagnostics.get());

	ASTActionFactory actionFactory(m_client, m_fileRegister, canonicalFilePathCache);
	tool.run(&actionFactory);
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

std::vector<std::string> CxxParser::getCommandlineArguments(std::shared_ptr<IndexerCommandCxxEmpty> indexerCommand) const
{
	std::vector<std::string> args = getCommandlineArgumentsEssential(
		indexerCommand->getCompilerFlags(), indexerCommand->getSystemHeaderSearchPaths(), indexerCommand->getFrameworkSearchPaths()
	);

	// Set language standard
	std::string standard = "-std=" + indexerCommand->getLanguageStandard();
	args.push_back(standard);

	return args;
}

std::shared_ptr<CxxDiagnosticConsumer> CxxParser::getDiagnostics(std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache, bool logErrors) const
{
	llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> options = new clang::DiagnosticOptions();
	return std::make_shared<CxxDiagnosticConsumer>(
		llvm::errs(), &*options, m_client, m_fileRegister, canonicalFilePathCache, logErrors);
}
