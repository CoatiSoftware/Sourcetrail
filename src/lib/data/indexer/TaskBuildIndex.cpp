#include "data/indexer/TaskBuildIndex.h"

#include "data/indexer/IndexerFactory.h"
#include "data/indexer/IndexerCommandList.h"
#include "data/indexer/IndexerComposite.h"
#include "data/StorageProvider.h"
#include "component/view/DialogView.h"
#include "utility/file/FileRegister.h"
#include "utility/file/FileRegisterStateData.h"
#include "utility/scheduling/Blackboard.h"
#include "ApplicationStateMonitor.h"
#include "Application.h"

TaskBuildIndex::TaskBuildIndex(
	std::shared_ptr<IndexerCommandList> indexerCommandList,
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegisterStateData> fileRegisterStateData
)
	: m_indexerCommandList(indexerCommandList)
	, m_storageProvider(storageProvider)
	, m_fileRegisterStateData(fileRegisterStateData)
{
	m_indexer = IndexerFactory::getInstance()->createCompositeIndexerForAllRegisteredModules();
}

void TaskBuildIndex::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	std::lock_guard<std::mutex> lock(blackboard->getMutex());

	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount++;
		blackboard->set("indexer_count", indexerCount);
	}
}

Task::TaskState TaskBuildIndex::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	std::shared_ptr<IndexerCommand> indexerCommand = m_indexerCommandList->consumeCommand();

	if (!indexerCommand)
	{
		return STATE_FAILURE;
	}
	else
	{
		ApplicationStateMonitor::getInstance()->addIndexingFile(indexerCommand->getSourceFilePath());

		{
			std::lock_guard<std::mutex> lock(blackboard->getMutex());

			int sourceFileCount = 0;
			blackboard->get("source_file_count", sourceFileCount);

			int indexedSourceFileCount = 0;
			blackboard->get("indexed_source_file_count", indexedSourceFileCount);

			if (std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView())
			{
				dialogView->updateIndexingDialog(
					indexedSourceFileCount, sourceFileCount, indexerCommand->getSourceFilePath().str()
				);
			}
		}

		// file register only copies the DileRegisterStateData
		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			*(m_fileRegisterStateData.get()), indexerCommand->getIndexedPaths(), indexerCommand->getExcludedPath()
		);

		std::shared_ptr<IntermediateStorage> storage = m_indexer->index(indexerCommand, fileRegister);
		if (storage)
		{
			// only write back the changes made to FileRegisterStateData if the indexer actually succeeded
			m_fileRegisterStateData->inject(fileRegister->getStateData());

			m_storageProvider->insert(storage);

			std::lock_guard<std::mutex> lock(blackboard->getMutex());
			int indexedSourceFileCount = 0;
			blackboard->get("indexed_source_file_count", indexedSourceFileCount);
			blackboard->set("indexed_source_file_count", indexedSourceFileCount + 1);
		}

		ApplicationStateMonitor::getInstance()->removeIndexingFile(indexerCommand->getSourceFilePath());
	}

	return (m_indexer->interrupted() ? STATE_FAILURE : STATE_SUCCESS);
}

void TaskBuildIndex::doExit(std::shared_ptr<Blackboard> blackboard)
{
	std::lock_guard<std::mutex> lock(blackboard->getMutex());

	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount--;
		blackboard->set("indexer_count", indexerCount);
	}
}

void TaskBuildIndex::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskBuildIndex::handleMessage(MessageInterruptTasks* message)
{
	m_indexer->interrupt();
}
