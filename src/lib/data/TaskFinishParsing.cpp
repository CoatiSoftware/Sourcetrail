#include "data/TaskFinishParsing.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/utility.h"

TaskFinishParsing::TaskFinishParsing(
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister,
	DialogView* dialogView
)
	: m_storage(storage)
	, m_fileRegister(fileRegister)
	, m_dialogView(dialogView)
{
}

TaskFinishParsing::~TaskFinishParsing()
{
}

void TaskFinishParsing::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskFinishParsing::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	TimePoint start = utility::durationStart();

	m_dialogView->showProgressDialog("Finish Indexing", "Optimizing database");
	m_storage->optimizeMemory();

	m_dialogView->showProgressDialog("Finish Indexing", "Building caches");
	m_storage->buildCaches();

	m_dialogView->hideProgressDialog();
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

	m_dialogView->finishedIndexingDialog(
		m_fileRegister->getParsedSourceFilesCount(),
		m_fileRegister->getSourceFilesCount(),
		time,
		m_storage->getErrorCount()
	);

	return STATE_SUCCESS;
}

void TaskFinishParsing::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskFinishParsing::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
