#include "data/TaskFinishParsing.h"

#include "component/view/DialogView.h"
#include "data/storage/PersistentStorage.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageQuitApplication.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
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

	dialogView->showUnknownProgressDialog(L"Finish Indexing", L"Optimizing database");
	m_storage->optimizeMemory();

	dialogView->showUnknownProgressDialog(L"Finish Indexing", L"Building caches");
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

	std::wstring status;
	status += L"Finished indexing: ";
	status += std::to_wstring(indexedSourceFileCount) + L"/" + std::to_wstring(sourceFileCount) + L" source files indexed; ";
	status += utility::decodeFromUtf8(utility::timeToString(time));
	status += L"; " + std::to_wstring(errorInfo.total) + L" error" + (errorInfo.total != 1 ? L"s" : L"");
	if (errorInfo.fatal > 0)
	{
		status += L" (" + std::to_wstring(errorInfo.fatal) + L" fatal)";
	}
	MessageStatus(status, false, false).dispatch();

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

	MessageStatus(L"An unknown exception was thrown during indexing.", true, false).dispatch();
	MessageFinishedParsing().dispatch();
}
