#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/cxx/CxxCompilationDatabaseSingle.h"
#include "data/parser/Parser.h"

class CxxDiagnosticConsumer;
class FileManager;
class FileRegister;
class TaskParseCxx;

class CxxParser: public Parser
{
public:
	CxxParser(ParserClient* client, const FileManager* fileManager);
	~CxxParser();

	// ParserClient implementation
	virtual void parseFiles(const std::vector<FilePath>& filePaths, const Arguments& arguments);
	virtual void parseFile(const FilePath& filePath, std::shared_ptr<TextAccess> textAccess, const Arguments& arguments);

private:
	std::vector<std::string> getCommandlineArgumentsEssential(const Arguments& arguments) const;
	std::vector<std::string> getCommandlineArguments(const Arguments& arguments) const;
	std::shared_ptr<clang::tooling::FixedCompilationDatabase> getCompilationDatabase(const Arguments& arguments) const;

	std::shared_ptr<CxxDiagnosticConsumer> getDiagnostics(const Arguments& arguments) const;

	// Accessed by TaskParseCxx
	void setupParsing(const std::vector<FilePath>& filePaths, const Arguments& arguments);
	void setupParsingCDB(const std::vector<FilePath>& filePaths, const Arguments& arguments);

	void runTool(const std::vector<std::string>& files);
	void runTool(clang::tooling::CompileCommand command, const Arguments& arguments);

	FileRegister* getFileRegister();
	ParserClient* getParserClient();

	std::string getLanguageArgument(const std::string& language) const;

	friend class TaskParseCxx;

	std::shared_ptr<FileRegister> m_fileRegister;

	std::shared_ptr<clang::tooling::CompilationDatabase> m_compilationDatabase;
	std::shared_ptr<CxxDiagnosticConsumer> m_diagnostics;
};

#endif // CXX_PARSER_H
