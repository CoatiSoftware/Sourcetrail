#include "data/TaskShowDialogView.h"

#include "component/view/DialogView.h"

TaskShowDialogView::TaskShowDialogView(
	const std::string& title,
	const std::string& message,
	DialogView* dialogView
)
	: m_title(title)
	, m_message(message)
	, m_dialogView(dialogView)
{
}

TaskShowDialogView::~TaskShowDialogView()
{
}

void TaskShowDialogView::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskShowDialogView::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	m_dialogView->showProgressDialog(m_title, m_message);
	return STATE_SUCCESS;
}

void TaskShowDialogView::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskShowDialogView::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
