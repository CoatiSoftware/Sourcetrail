#ifndef TASK_SHOW_UNKNOWN_PROGRESS_DIALOG_H
#define TASK_SHOW_UNKNOWN_PROGRESS_DIALOG_H

#include <string>

#include "utility/scheduling/Task.h"

class TaskShowUnknownProgressDialog
	: public Task
{
public:
	TaskShowUnknownProgressDialog(
		const std::wstring& title,
		const std::wstring& message
	);

	virtual ~TaskShowUnknownProgressDialog();

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	const std::wstring m_title;
	const std::wstring m_message;
};

#endif // TASK_SHOW_UNKNOWN_PROGRESS_DIALOG_H
