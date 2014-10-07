#include "data/parser/cxx/CxxParser.h"

#include "data/parser/cxx/ASTActionFactory.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"

CxxParser::CxxParser(ParserClient* client)
	: Parser(client)
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

	ASTActionFactory actionFactory(m_client);

	tool.run(&actionFactory);
}

void CxxParser::parseFile(std::shared_ptr<TextAccess> textAccess)
{
	ASTActionFactory actionFactory(m_client);
	clang::tooling::runToolOnCode(actionFactory.create(), textAccess->getText());
}
