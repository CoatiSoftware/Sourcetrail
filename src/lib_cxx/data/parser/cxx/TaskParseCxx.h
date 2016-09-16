#ifndef TASK_PARSE_CXX_H
#define TASK_PARSE_CXX_H

#include <memory>
#include <deque>

#include "data/parser/Parser.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/scheduling/Task.h"
#include "utility/TimePoint.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/MessageListener.h"

class CxxParser;
class DialogView;
class FileRegister;
class StorageProvider;

namespace clang
{
	namespace tooling
	{
		class JSONCompilationDatabase;
	}
}

class TaskParseCxx
	: public Task
	, public MessageListener<MessageInterruptTasks>
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
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	virtual void handleMessage(MessageInterruptTasks* message);

	std::shared_ptr<StorageProvider> m_storageProvider;

	const Parser::Arguments m_arguments;
	DialogView* m_dialogView;

	std::shared_ptr<CxxParser> m_parser;
	std::shared_ptr<ParserClientImpl> m_parserClient;

	bool m_isCDB;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_cdb;

	bool m_interrupted;
};

#endif // TASK_PARSE_CXX_H
