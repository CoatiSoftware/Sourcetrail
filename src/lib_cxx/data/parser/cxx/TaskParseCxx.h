#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include "data/parser/TaskParse.h"

class CxxParser;
class ParserClientImpl;

namespace clang
{
	namespace tooling
	{
		class JSONCompilationDatabase;
	}
}

class TaskParseCxx
	: public TaskParse
{
public:
	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);

	TaskParseCxx(
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<FileRegister> fileRegister,
		const Parser::Arguments& arguments,
		DialogView* dialogView
	);

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);

	virtual void indexFile(FilePath sourcePath);

	std::shared_ptr<CxxParser> m_parser;
	std::shared_ptr<ParserClientImpl> m_parserClient;

	bool m_isCDB;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_cdb;
};

#endif // TASK_PARSE_CXX_H
