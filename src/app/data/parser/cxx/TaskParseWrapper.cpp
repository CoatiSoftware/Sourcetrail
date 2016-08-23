#include "data/parser/cxx/TaskParseWrapper.h"

#include "component/view/DialogView.h"
#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/utility.h"

TaskParseWrapper::TaskParseWrapper(
	PersistentStorage* storage,
	std::shared_ptr<FileRegister> fileRegister,
	DialogView* dialogView
)
	: m_storage(storage)
	, m_fileRegister(fileRegister)
	, m_dialogView(dialogView)
{
}

TaskParseWrapper::~TaskParseWrapper()
{
}

void TaskParseWrapper::enter()
{
	m_dialogView->updateIndexingDialog(0, m_fileRegister->getSourceFilesCount(), "");

	m_start = utility::durationStart();
	m_storage->startParsing();

	m_task->enter();
}

Task::TaskState TaskParseWrapper::update()
{
	return m_task->update();
}

void TaskParseWrapper::exit()
{
	m_task->exit();

	m_dialogView->showProgressDialog("Finish Indexing", "Optimizing database");

	m_storage->optimizeMemory();

	m_dialogView->showProgressDialog("Finish Indexing", "Building caches");

	m_storage->finishParsing();

	m_dialogView->hideProgressDialog();

	MessageFinishedParsing().dispatch();

	m_dialogView->finishedIndexingDialog(
		m_fileRegister->getParsedSourceFilesCount(),
		m_fileRegister->getSourceFilesCount(),
		utility::duration(m_start),
		m_storage->getErrorCount()
	);
}

void TaskParseWrapper::interrupt()
{
	m_task->interrupt();
}

void TaskParseWrapper::revert()
{
	m_task->revert();
}

void TaskParseWrapper::abort()
{
	m_task->abort();

	MessageFinishedParsing().dispatch();

	m_dialogView->finishedIndexingDialog(
		m_fileRegister->getParsedSourceFilesCount(),
		m_fileRegister->getSourceFilesCount(),
		0,
		m_storage->getErrorCount()
	);
}
