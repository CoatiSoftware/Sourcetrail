#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include <queue>

#include "utility/scheduling/Task.h"
#include "utility/utility.h"

#include "data/parser/cxx/CxxParser.h"

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
	CxxParser m_parser;
	const Parser::Arguments m_arguments;
	const std::vector<FilePath> m_files;

	std::queue<std::string> m_sourcePaths;

	utility::TimePoint m_start;
};

#endif // TASK_PARSE_CXX_H
