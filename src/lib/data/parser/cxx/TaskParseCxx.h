#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include <memory>
#include <deque>

#include "data/parser/Parser.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class Storage;
class FileManager;
class CxxParser;

namespace clang
{
	namespace tooling
	{
		class JSONCompilationDatabase;
	}
}

class TaskParseCxx
	: public Task
{
public:
	TaskParseCxx(
		Storage* storage,
		const FileManager* fileManager,
		const Parser::Arguments& arguments,
		const std::vector<FilePath>& files
	);

	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	Storage* m_storage;
	std::shared_ptr<CxxParser> m_parser;
	std::shared_ptr<ParserClientImpl> m_parserClient;
	const Parser::Arguments m_arguments;
	const std::vector<FilePath> m_files;

	std::deque<FilePath> m_sourcePaths;

	TimePoint m_start;

	bool m_isCDB;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_cdb;
};

#endif // TASK_PARSE_CXX_H
