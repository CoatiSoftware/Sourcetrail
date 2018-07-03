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

private:
	void doEnter(std::shared_ptr<Blackboard> blackboard) override;
	TaskState doUpdate(std::shared_ptr<Blackboard> blackboard) override;
	void doExit(std::shared_ptr<Blackboard> blackboard) override;
	void doReset(std::shared_ptr<Blackboard> blackboard) override;

	const std::wstring m_title;
	const std::wstring m_message;
};

#endif // TASK_SHOW_UNKNOWN_PROGRESS_DIALOG_H
