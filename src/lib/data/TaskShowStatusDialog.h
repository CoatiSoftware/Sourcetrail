#ifndef TASK_SHOW_STATUS_DIALOG_H
#define TASK_SHOW_STATUS_DIALOG_H

#include <string>

#include "utility/scheduling/Task.h"

class DialogView;

class TaskShowStatusDialog
	: public Task
{
public:
	TaskShowStatusDialog(
		const std::string& title,
		const std::string& message
	);

	virtual ~TaskShowStatusDialog();

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	const std::string m_title;
	const std::string m_message;
};

#endif // TASK_SHOW_STATUS_DIALOG_H
