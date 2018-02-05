#include "InterprocessIndexer.h"

#include "utility/file/FileRegister.h"
#include "utility/file/FileRegisterStateData.h"
#include "utility/logging/logging.h"

#include "data/indexer/IndexerCommand.h"
#include "data/indexer/IndexerComposite.h"
#include "data/indexer/IndexerFactory.h"

InterprocessIndexer::InterprocessIndexer(const std::string& uuid, Id processId)
	: m_interprocessIndexerCommandManager(uuid, processId, false)
	, m_interprocessIndexingStatusManager(uuid, processId, false)
	, m_interprocessIntermediateStorageManager(uuid, processId, false)
	, m_uuid(uuid)
	, m_processId(processId)
{
}

InterprocessIndexer::~InterprocessIndexer()
{
}

void InterprocessIndexer::work()
{
	try
	{
		LOG_INFO(std::to_wstring(m_processId) + L" starting up indexer");
		std::shared_ptr<IndexerBase> indexer = IndexerFactory::getInstance()->createCompositeIndexerForAllRegisteredModules();

		while (std::shared_ptr<IndexerCommand> indexerCommand = m_interprocessIndexerCommandManager.popIndexerCommand())
		{
			LOG_INFO(std::to_wstring(m_processId) + L" fetched indexer command for \"" + indexerCommand->getSourceFilePath().wstr() + L"\"");
			LOG_INFO(std::to_wstring(m_processId) + L" indexer commands left: " + std::to_wstring(m_interprocessIndexerCommandManager.indexerCommandCount() + 1));

			while (true)
			{
				size_t storageCount = m_interprocessIntermediateStorageManager.getIntermediateStorageCount();
				if (storageCount < 10)
				{
					break;
				}

				LOG_INFO_STREAM(<< m_processId << " waits, too many intermediate storages: " << storageCount);

				const int SLEEP_TIME_MS = 200;
				std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
			}

			LOG_INFO_STREAM(<< m_processId << " updating indexer status with currently indexed filepath");
			m_interprocessIndexingStatusManager.startIndexingSourceFile(indexerCommand->getSourceFilePath());

			FileRegisterStateData data;
			data.setIndexedFiles(m_interprocessIndexingStatusManager.getIndexedFiles());

			std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
				data, indexerCommand->getSourceFilePath(), indexerCommand->getIndexedPaths(), indexerCommand->getExcludedPath()
			);

			LOG_INFO_STREAM(<< m_processId << " starting to index current file");
			std::shared_ptr<IntermediateStorage> result = indexer->index(indexerCommand, fileRegister);

			LOG_INFO_STREAM(<< m_processId << " finished indexing current file, updating indexer status");
			m_interprocessIndexingStatusManager.addIndexedFiles(fileRegister->getStateData().getIndexedFiles());

			LOG_INFO_STREAM(<< m_processId << " pushing index to shared memory");
			m_interprocessIntermediateStorageManager.pushIntermediateStorage(result);

			LOG_INFO_STREAM(<< m_processId << " finalizing indexer status for current file");
			m_interprocessIndexingStatusManager.finishIndexingSourceFile();

			LOG_INFO_STREAM(<< m_processId << " all done");
		}
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR(e.what());
		throw e;
	}
	catch (std::exception& e)
	{
		LOG_ERROR(e.what());
		throw e;
	}
	catch (...)
	{
		LOG_ERROR("something went wrong while running the indexer");
	}

	LOG_INFO_STREAM(<< "shutting down indexer");
}
