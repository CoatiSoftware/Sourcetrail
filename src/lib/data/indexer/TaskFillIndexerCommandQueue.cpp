#include "data/indexer/TaskFillIndexerCommandQueue.h"

#include "data/indexer/IndexerCommandProvider.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/scheduling/Blackboard.h"

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
	typedef std::pair<unsigned long long int, FilePath> PairType;
	std::vector<PairType> sourceFileSizesToCommands;
	{
		std::vector<FilePath> allSourceFilePaths;
		{
			std::lock_guard<std::mutex> lock(m_commandsMutex);
			allSourceFilePaths = m_indexerCommandProvider->getAllSourceFilePaths();
		}


		for (const FilePath& path : allSourceFilePaths)
		{
			if (path.exists())
			{
				sourceFileSizesToCommands.push_back(std::make_pair(FileSystem::getFileByteSize(path), path));
			}
			else
			{
				sourceFileSizesToCommands.push_back(std::make_pair(1, path));
			}
		}
		std::sort(sourceFileSizesToCommands.begin(), sourceFileSizesToCommands.end(), [](const PairType& p, const PairType& q) { return p.first > q.first; });

		if (sourceFileSizesToCommands.size() > 2)
		{
			std::sort(
				sourceFileSizesToCommands.begin(),
				sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() / 2,
				[](const PairType& p, const PairType& q) { return p.second.wstr() < q.second.wstr(); }
			);
			std::sort(
				sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() / 2,
				sourceFileSizesToCommands.end(),
				[](const PairType& p, const PairType& q) { return p.second.wstr() < q.second.wstr(); }
			);
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_commandsMutex);
		for (const PairType &pair : sourceFileSizesToCommands)
		{
			m_filePathQueue.emplace(pair.second);
		}
	}

	fillCommandQueue();

	blackboard->set<bool>("indexer_command_queue_started", true);
}

Task::TaskState TaskFillIndexerCommandsQueue::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	fillCommandQueue();

	{
		std::lock_guard<std::mutex> lock(m_commandsMutex);

		if (m_indexerCommandProvider->size() == 0)
		{
			return STATE_SUCCESS;
		}
	}

	const int SLEEP_TIME_MS = 200;
	std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));

	return STATE_RUNNING;
}

void TaskFillIndexerCommandsQueue::doExit(std::shared_ptr<Blackboard> blackboard)
{
	blackboard->set<bool>("indexer_command_queue_stopped", true);
}

void TaskFillIndexerCommandsQueue::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFillIndexerCommandsQueue::handleMessage(MessageInterruptTasks* message)
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	LOG_INFO("Discarding remaining " + std::to_string(m_indexerCommandProvider->size() + m_indexerCommandManager.indexerCommandCount()) + " indexer commands.");
	std::queue<FilePath> empty;
	std::swap(m_filePathQueue, empty);
	m_indexerCommandProvider->clear();
	m_indexerCommandManager.clearIndexerCommands();
	LOG_INFO("Remaining: " + std::to_string(m_indexerCommandProvider->size() + m_indexerCommandManager.indexerCommandCount()) + ".");
}

void TaskFillIndexerCommandsQueue::fillCommandQueue()
{
	std::lock_guard<std::mutex> lock(m_commandsMutex);
	while (!m_indexerCommandProvider->empty() && m_indexerCommandManager.indexerCommandCount() < m_maximumQueueSize)
	{
		if (!m_filePathQueue.empty())
		{
			m_indexerCommandManager.pushIndexerCommands({ m_indexerCommandProvider->consumeCommandForSourceFilePath(m_filePathQueue.front()) });
			m_filePathQueue.pop();
		}
		else
		{
			m_indexerCommandManager.pushIndexerCommands({ m_indexerCommandProvider->consumeCommand() });
		}
	}
}
