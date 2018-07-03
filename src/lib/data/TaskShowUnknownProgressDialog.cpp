#include "data/TaskShowUnknownProgressDialog.h"

#include "component/view/DialogView.h"
#include "Application.h"

TaskShowUnknownProgressDialog::TaskShowUnknownProgressDialog(
	const std::wstring& title,
	const std::wstring& message
)
	: m_title(title)
	, m_message(message)
{
}

void TaskShowUnknownProgressDialog::doEnter(std::shared_ptr<Blackboard> blackboard)
{
}

Task::TaskState TaskShowUnknownProgressDialog::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView())
	{
		dialogView->showUnknownProgressDialog(m_title, m_message);
	}
	return STATE_SUCCESS;
}

void TaskShowUnknownProgressDialog::doExit(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskShowUnknownProgressDialog::doReset(std::shared_ptr<Blackboard> blackboard)
{
}
