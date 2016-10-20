#include "data/parser/TaskParse.h"

#include "utility/scheduling/Blackboard.h"

TaskParse::TaskParse(
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: m_storageProvider(storageProvider)
	, m_fileRegister(fileRegister)
	, m_arguments(arguments)
	, m_dialogView(dialogView)
	, m_interrupted(false)
{
}

void TaskParse::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	std::lock_guard<std::mutex> lock(blackboard->getMutex());

	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount++;
		blackboard->set("indexer_count", indexerCount);
	}
}

void TaskParse::doExit(std::shared_ptr<Blackboard> blackboard)
{
	std::lock_guard<std::mutex> lock(blackboard->getMutex());

	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount--;
		blackboard->set("indexer_count", indexerCount);
	}
}

void TaskParse::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParse::handleMessage(MessageInterruptTasks* message)
{
	m_interrupted = true;
}
