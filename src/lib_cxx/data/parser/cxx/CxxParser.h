#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/cxx/cxxCacheTypes.h"
#include "data/parser/cxx/CxxCompilationDatabaseSingle.h"
#include "data/parser/Parser.h"

class CxxDiagnosticConsumer;
class FilePath;
class FileRegister;
class IndexerCommandCxxCdb;
class IndexerCommandCxxManual;
class TaskParseCxx;

class CxxParser: public Parser
{
public:
	CxxParser(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister);
	~CxxParser();

	void buildIndex(std::shared_ptr<IndexerCommandCxxCdb> indexerCommand);
	void buildIndex(std::shared_ptr<IndexerCommandCxxManual> indexerCommand);
	void buildIndex(const std::string& fileName, std::shared_ptr<TextAccess> fileContent);

private:
	std::vector<std::string> getCommandlineArgumentsEssential(
		const std::vector<std::string>& compilerFlags,
		const std::vector<FilePath>& systemHeaderSearchPaths,
		const std::vector<FilePath>& frameworkSearchPaths) const;
	std::vector<std::string> getCommandlineArguments(std::shared_ptr<IndexerCommandCxxManual> indexerCommand) const;
	std::shared_ptr<clang::tooling::FixedCompilationDatabase> getCompilationDatabase(std::shared_ptr<IndexerCommandCxxManual> indexerCommand) const;

	std::shared_ptr<CxxDiagnosticConsumer> getDiagnostics(std::shared_ptr<FilePathCache> canonicalFilePathCache, bool logErrors) const;

	friend class TaskParseCxx;

	std::shared_ptr<FileRegister> m_fileRegister;

};

#endif // CXX_PARSER_H
