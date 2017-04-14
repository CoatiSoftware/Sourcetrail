#include "data/TaskFinishParsing.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"
#include "Application.h"

TaskFinishParsing::TaskFinishParsing(
	PersistentStorage* storage,
	StorageAccess* storageAccess
)
	: m_storage(storage)
	, m_storageAccess(storageAccess)
{
}

TaskFinishParsing::~TaskFinishParsing()
{
}

void TaskFinishParsing::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_storage->setMode(SqliteStorage::STORAGE_MODE_READ);
}

Task::TaskState TaskFinishParsing::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TimePoint start = utility::durationStart();

	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	if (dialogView)
	{
		dialogView->showStatusDialog("Finish Indexing", "Optimizing database");
	}
	m_storage->optimizeMemory();

	if (dialogView)
	{
		dialogView->showStatusDialog("Finish Indexing", "Building caches");
	}
	m_storage->buildCaches();

	if (dialogView)
	{
		dialogView->hideStatusDialog();
	}
	MessageFinishedParsing().dispatch();

	float time = utility::duration(start);

	if (blackboard->exists("clear_time"))
	{
		float clearTime = 0;
		blackboard->get("clear_time", clearTime);
		time += clearTime;
	}

	if (blackboard->exists("index_time"))
	{
		float indexTime = 0;
		blackboard->get("index_time", indexTime);
		time += indexTime;
	}

	int indexedSourceFileCount = 0;
	blackboard->get("indexed_source_file_count", indexedSourceFileCount);

	int sourceFileCount = 0;
	blackboard->get("source_file_count", sourceFileCount);

	if (dialogView)
	{
		StorageStats stats = m_storageAccess->getStorageStats();

		dialogView->finishedIndexingDialog(
			indexedSourceFileCount,
			sourceFileCount,
			stats.completedFileCount,
			stats.fileCount,
			time,
			m_storageAccess->getErrorCount()
		);
	}

	return STATE_SUCCESS;
}

void TaskFinishParsing::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFinishParsing::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
