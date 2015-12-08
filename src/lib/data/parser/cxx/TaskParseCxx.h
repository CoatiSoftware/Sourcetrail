#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include <memory>
#include <queue>

#include "data/parser/Parser.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"

class ParserClient;
class FileManager;
class CxxParser;

class TaskParseCxx
	: public Task
{
public:
	TaskParseCxx(
		ParserClient* client,
		const FileManager* fileManager,
		const Parser::Arguments& arguments,
		const std::vector<FilePath>& files
	);

	virtual void enter();
	virtual TaskState update();
	virtual void exit();

	virtual void interrupt();
	virtual void revert();

private:
	ParserClient* m_client;
	std::shared_ptr<CxxParser> m_parser;
	const Parser::Arguments m_arguments;
	const std::vector<FilePath> m_files;

	std::queue<FilePath> m_sourcePaths;

	TimePoint m_start;
};

#endif // TASK_PARSE_CXX_H
