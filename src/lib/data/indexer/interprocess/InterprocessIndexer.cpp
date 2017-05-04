#include "InterprocessIndexer.h"

#include "utility/file/FileRegister.h"
#include "utility/file/FileRegisterStateData.h"
#include "utility/logging/logging.h"

#include "data/indexer/IndexerComposite.h"
#include "data/indexer/IndexerCommand.h"
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
		LOG_INFO_STREAM(<< m_processId << " Starting to index");
		std::shared_ptr<IndexerBase> indexer = IndexerFactory::getInstance()->createCompositeIndexerForAllRegisteredModules();

		while (std::shared_ptr<IndexerCommand> indexerCommand = m_interprocessIndexerCommandManager.popIndexerCommand())
		{
			LOG_INFO_STREAM(<< m_processId << " Indexing " << indexerCommand->getSourceFilePath().str());
			LOG_INFO_STREAM(<< m_processId << " Commands left: " << (m_interprocessIndexerCommandManager.indexerCommandCount() + 1));

			m_interprocessIndexingStatusManager.setCurrentlyIndexedSourceFilePath(indexerCommand->getSourceFilePath());

			FileRegisterStateData data;
			data.setIndexedFiles(m_interprocessIndexingStatusManager.getIndexedFiles());

			std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
				data, indexerCommand->getIndexedPaths(), indexerCommand->getExcludedPath()
			);

			std::shared_ptr<IntermediateStorage> result = indexer->index(indexerCommand, fileRegister);

			m_interprocessIndexingStatusManager.addIndexedFiles(fileRegister->getStateData().getIndexedFiles());

			m_interprocessIntermediateStorageManager.pushIntermediateStorage(result);

			m_interprocessIndexingStatusManager.clearCurrentlyIndexedSourceFilePath();
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

	LOG_INFO_STREAM(<< "Finished indexing");
}
