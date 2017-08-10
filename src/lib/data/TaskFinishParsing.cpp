#include "data/TaskFinishParsing.h"

#include "component/view/DialogView.h"
#include "data/storage/PersistentStorage.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageQuitApplication.h"
#include "utility/messaging/type/MessageStatus.h"
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
	m_storage->setMode(SqliteIndexStorage::STORAGE_MODE_READ);
}

Task::TaskState TaskFinishParsing::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TimeStamp start = utility::durationStart();

	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	dialogView->showUnknownProgressDialog("Finish Indexing", "Optimizing database");
	m_storage->optimizeMemory();

	dialogView->showUnknownProgressDialog("Finish Indexing", "Building caches");
	m_storage->buildCaches();

	dialogView->hideUnknownProgressDialog();
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

	bool interruptedIndexing = false;
	blackboard->get("interrupted_indexing", interruptedIndexing);

	ErrorCountInfo errorInfo = m_storageAccess->getErrorCount();

	std::stringstream ss;
	ss << "Finished indexing: ";
	ss << indexedSourceFileCount << "/" << sourceFileCount << " source files indexed; ";
	ss << utility::timeToString(time);
	ss << "; " << errorInfo.total << " error" << (errorInfo.total != 1 ? "s" : "");
	if (errorInfo.fatal > 0)
	{
		ss << " (" << errorInfo.fatal << " fatal)";
	}
	MessageStatus(ss.str(), false, false).dispatch();

	StorageStats stats = m_storageAccess->getStorageStats();
	dialogView->finishedIndexingDialog(
		indexedSourceFileCount,
		sourceFileCount,
		stats.completedFileCount,
		stats.fileCount,
		time,
		errorInfo,
		interruptedIndexing
	);

	return STATE_SUCCESS;
}

void TaskFinishParsing::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFinishParsing::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFinishParsing::terminate()
{
	Application* app = Application::getInstance().get();
	if (app)
	{
		app->getDialogView()->hideDialogs();
	}

	MessageStatus("An unknown exception was thrown during indexing.", true, false).dispatch();
	MessageFinishedParsing().dispatch();
}
