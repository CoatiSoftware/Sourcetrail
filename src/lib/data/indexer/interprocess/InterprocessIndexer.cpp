#include "InterprocessIndexer.h"

#include "FileRegister.h"
#include "IndexerCommand.h"
#include "IndexerComposite.h"
#include "LanguagePackageManager.h"
#include "ScopedFunctor.h"
#include "logging.h"

InterprocessIndexer::InterprocessIndexer(const std::string& uuid, Id processId)
	: m_interprocessIndexerCommandManager(uuid, processId, false)
	, m_interprocessIndexingStatusManager(uuid, processId, false)
	, m_interprocessIntermediateStorageManager(uuid, processId, false)
	, m_uuid(uuid)
	, m_processId(processId)
{
}

void InterprocessIndexer::work()
{
	bool updaterThreadRunning = true;
	std::shared_ptr<std::thread> updaterThread;
	std::shared_ptr<IndexerBase> indexer;

	try
	{
		LOG_INFO_STREAM(<< m_processId << " starting up indexer");
		indexer = LanguagePackageManager::getInstance()->instantiateSupportedIndexers();

		updaterThread = std::make_shared<std::thread>([&]() {
			while (updaterThreadRunning)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));

				if (m_interprocessIndexingStatusManager.getIndexingInterrupted())
				{
					LOG_INFO_STREAM(<< m_processId << " received indexer interrupt command.");
					if (indexer)
					{
						indexer->interrupt();
					}
					updaterThreadRunning = false;
				}
			}
		});

		ScopedFunctor threadStopper([&]() {
			updaterThreadRunning = false;
			if (updaterThread)
			{
				updaterThread->join();
				updaterThread.reset();
			}
		});

		while (std::shared_ptr<IndexerCommand> indexerCommand =
				   m_interprocessIndexerCommandManager.popIndexerCommand())
		{
			LOG_INFO_STREAM(
				<< m_processId << " fetched indexer command for \""
				<< indexerCommand->getSourceFilePath().str() << "\"");
			LOG_INFO_STREAM(
				<< m_processId << " indexer commands left: "
				<< m_interprocessIndexerCommandManager.indexerCommandCount());

			while (updaterThreadRunning)
			{
				const size_t storageCount =
					m_interprocessIntermediateStorageManager.getIntermediateStorageCount();
				if (storageCount < 2)
				{
					break;
				}

				LOG_INFO_STREAM(<< m_processId << " waits, too many intermediate storages: " << storageCount);

				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			if (!updaterThreadRunning)
			{
				break;
			}

			LOG_INFO_STREAM(<< m_processId << " updating indexer status with currently indexed filepath");
			m_interprocessIndexingStatusManager.startIndexingSourceFile(
				indexerCommand->getSourceFilePath());

			LOG_INFO_STREAM(<< m_processId << " starting to index current file");
			std::shared_ptr<IntermediateStorage> result = indexer->index(indexerCommand);

			if (result)
			{
				LOG_INFO_STREAM(<< m_processId << " pushing index to shared memory");
				m_interprocessIntermediateStorageManager.pushIntermediateStorage(result);
			}

			LOG_INFO_STREAM(<< m_processId << " finalizing indexer status for current file");
			m_interprocessIndexingStatusManager.finishIndexingSourceFile();

			LOG_INFO_STREAM(<< m_processId << " all done");
		}
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR_STREAM(<< m_processId << " error: " << e.what());
		throw e;
	}
	catch (std::exception& e)
	{
		LOG_ERROR_STREAM(<< m_processId << " error: " << e.what());
		throw e;
	}
	catch (...)
	{
		LOG_ERROR_STREAM(<< m_processId << " something went wrong while running the indexer");
	}

	LOG_INFO_STREAM(<< m_processId << " shutting down indexer");
}
