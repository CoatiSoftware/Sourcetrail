#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include <memory>
#include <mutex>
#include <deque>

#include "data/parser/Parser.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class PersistentStorage;
class FileRegister;
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
	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);

	TaskParseCxx(
		PersistentStorage* storage,
		std::shared_ptr<std::mutex> storageMutex,
		std::shared_ptr<FileRegister> fileRegister,
		const Parser::Arguments& arguments
	);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	PersistentStorage* m_storage;
	std::shared_ptr<std::mutex> m_storageMutex;
	std::shared_ptr<CxxParser> m_parser;
	std::shared_ptr<ParserClientImpl> m_parserClient;
	const Parser::Arguments m_arguments;

	bool m_isCDB;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_cdb;
};

#endif // TASK_PARSE_CXX_H
