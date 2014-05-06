#include "data/parser/cxx/CxxParser.h"

#include "data/parser/cxx/ASTActionFactory.h"
#include "utility/logging/logging.h"

CxxParser::CxxParser(std::shared_ptr<ParserClient> client)
	: Parser(client)
{
}

CxxParser::~CxxParser()
{
}

void CxxParser::parseFiles(const std::vector<std::string>& filePaths)
{
	const char* argv[] = { "app", "--" };
	int argc = 2;

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
