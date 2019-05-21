#include "TaskFillIndexerCommandQueue.h"

#include "Blackboard.h"
#include "FileSystem.h"
#include "IndexerCommandProvider.h"
#include "logging.h"
#include "utilityFile.h"

TaskFillIndexerCommandsQueue::TaskFillIndexerCommandsQueue(
	const std::string& appUUID,
	std::unique_ptr<IndexerCommandProvider> indexerCommandProvider,
	size_t maximumQueueSize
)
	: m_indexerCommandProvider(std::move(indexerCommandProvider))
	, m_indexerCommandManager(appUUID, 0, true)
	, m_maximumQueueSize(maximumQueueSize)
{
}

void TaskFillIndexerCommandsQueue::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	{
		std::lock_guard<std::mutex> lock(m_commandsMutex);
		for (const FilePath& filePath : utility::partitionFilePathsBySize(m_indexerCommandProvider->getAllSourceFilePaths(), 2))
		{
			m_filePathQueue.emplace(filePath);
		}
	}

	fillCommandQueue();

	blackboard->set<bool>("indexer_command_queue_started", true);
}

Task::TaskState TaskFillIndexerCommandsQueue::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_interrupted)
	{
		return STATE_FAILURE;
	}

	if (!fillCommandQueue())
	{
		std::lock_guard<std::mutex> lock(m_commandsMutex);

		if (m_indexerCommandProvider->empty())
		{
			return STATE_SUCCESS;
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	return STATE_RUNNING;
}

void TaskFillIndexerCommandsQueue::doExit(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set<bool>("indexer_command_queue_stopped", true);
}

void TaskFillIndexerCommandsQueue::doReset(std::shared_ptr<Blackboard> blackboard)
{
	m_interrupted = false;
}

void TaskFillIndexerCommandsQueue::terminate()
{
	m_interrupted = true;
}

void TaskFillIndexerCommandsQueue::handleMessage(MessageIndexingInterrupted* message)
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);

	LOG_INFO("Discarding remaining " + std::to_string(m_indexerCommandProvider->size() + m_indexerCommandManager.indexerCommandCount()) + " indexer commands.");

	std::queue<FilePath> empty;
	std::swap(m_filePathQueue, empty);

	m_indexerCommandProvider->clear();
	m_indexerCommandManager.clearIndexerCommands();

	LOG_INFO("Remaining: " + std::to_string(m_indexerCommandProvider->size() + m_indexerCommandManager.indexerCommandCount()) + ".");
}

bool TaskFillIndexerCommandsQueue::fillCommandQueue()
{
	size_t refillAmount = m_maximumQueueSize - m_indexerCommandManager.indexerCommandCount();
	if (!refillAmount)
	{
		return false;
	}

	std::lock_guard<std::mutex> lock(m_commandsMutex);
	std::vector<std::shared_ptr<IndexerCommand>> commands;

	while (!m_indexerCommandProvider->empty() && commands.size() < refillAmount)
	{
		if (!m_filePathQueue.empty())
		{
			commands.push_back(m_indexerCommandProvider->consumeCommandForSourceFilePath(m_filePathQueue.front()));
			m_filePathQueue.pop();
		}
		else
		{
			commands.push_back(m_indexerCommandProvider->consumeCommand());
		}
	}

	if (commands.size())
	{
		m_indexerCommandManager.pushIndexerCommands(commands);
		return true;
	}

	return false;
}
