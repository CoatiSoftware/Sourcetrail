#ifndef TASK_SHOW_DIALOG_VIEW_H
#define TASK_SHOW_DIALOG_VIEW_H

#include <string>

#include "utility/scheduling/Task.h"

class DialogView;

class TaskShowDialogView
	: public Task
{
public:
	TaskShowDialogView(
		const std::string& title,
		const std::string& message,
		DialogView* dialogView
	);

	virtual ~TaskShowDialogView();

private:
	virtual void doEnter(std::shared_ptr<Blackboard> blackboard);
	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blackboard);
	virtual void doExit(std::shared_ptr<Blackboard> blackboard);
	virtual void doReset(std::shared_ptr<Blackboard> blackboard);

	const std::string m_title;
	const std::string m_message;
	DialogView* m_dialogView;
};

#endif // TASK_SHOW_DIALOG_VIEW_H
