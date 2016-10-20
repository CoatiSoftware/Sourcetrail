#ifndef TASK_PARSE_H
#define TASK_PARSE_H

#include "data/parser/Parser.h"
#include "utility/scheduling/Task.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/MessageListener.h"

class CxxParser;
class DialogView;
class FileRegister;
class StorageProvider;

class TaskParse
	: public Task
	, public MessageListener<MessageInterruptTasks>
{
public:
	TaskParse(
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<FileRegister> fileRegister,
		const Parser::Arguments& arguments,
		DialogView* dialogView
	);

protected:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	virtual void handleMessage(MessageInterruptTasks* message);

	std::shared_ptr<StorageProvider> m_storageProvider;
	std::shared_ptr<FileRegister> m_fileRegister;

	const Parser::Arguments m_arguments;
	DialogView* m_dialogView;

	bool m_interrupted;
};

#endif // TASK_PARSE_H
