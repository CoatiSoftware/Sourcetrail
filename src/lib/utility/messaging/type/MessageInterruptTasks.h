#ifndef MESSAGE_INTERRUPT_TASKS_H
#define MESSAGE_INTERRUPT_TASKS_H

#include "utility/messaging/Message.h"

class MessageInterruptTasks:
	public Message<MessageInterruptTasks>
{
public:
	MessageInterruptTasks()
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageInterruptTasks";
	}
};

#endif // MESSAGE_INTERRUPT_TASKS_H
