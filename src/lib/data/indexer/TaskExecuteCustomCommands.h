#ifndef TASK_EXECUTE_CUSTOM_COMMANDS_H
#define TASK_EXECUTE_CUSTOM_COMMANDS_H

#include <vector>

#include "FilePath.h"
#include "Task.h"
#include "TimeStamp.h"
#include "MessageIndexingInterrupted.h"
#include "MessageListener.h"

class DialogView;
class IndexerCommandProvider;

class TaskExecuteCustomCommands
	: public Task
	, public MessageListener<MessageIndexingInterrupted>
{
public:
	TaskExecuteCustomCommands(
		std::unique_ptr<IndexerCommandProvider> indexerCommandProvider,
		std::shared_ptr<DialogView> dialogView,
		const FilePath& projectDirectory);

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	void handleMessage(MessageIndexingInterrupted* message) override;

	std::unique_ptr<IndexerCommandProvider> m_indexerCommandProvider;
	std::shared_ptr<DialogView> m_dialogView;
	const FilePath m_projectDirectory;

	TimeStamp m_start;
	bool m_interrupted = false;
};

#endif // TASK_EXECUTE_CUSTOM_COMMANDS_H
