#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include <string>
#include <vector>

#include "Parser.h"

class CanonicalFilePathCache;
class CxxDiagnosticConsumer;
class FilePath;
class FileRegister;
class IndexerCommandCxx;
class TaskParseCxx;
class TextAccess;

namespace clang {
	namespace tooling {
		class CompilationDatabase;
		class FixedCompilationDatabase;
	}
}

struct IndexerStateInfo;

class CxxParser: public Parser
{
public:
	static std::vector<std::string> getCommandlineArgumentsEssential(const std::vector<std::wstring>& compilerFlags);
	static void initializeLLVM();

	CxxParser(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister, std::shared_ptr<IndexerStateInfo> indexerStateInfo);

	void buildIndex(std::shared_ptr<IndexerCommandCxx> indexerCommand);
	void buildIndex(const std::wstring& fileName, std::shared_ptr<TextAccess> fileContent, std::vector<std::wstring> compilerFlags = {});

private:
	void runTool(clang::tooling::CompilationDatabase* compilationDatabase, const FilePath& sourceFilePath);

	std::shared_ptr<CxxDiagnosticConsumer> getDiagnostics(
		const FilePath& sourceFilePath, std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache, bool logErrors) const;

	friend class TaskParseCxx;

	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;

};

#endif // CXX_PARSER_H
