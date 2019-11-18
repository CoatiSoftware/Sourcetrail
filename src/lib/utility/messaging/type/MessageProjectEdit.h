#ifndef MESSAGE_PROJECT_EDIT_H
#define MESSAGE_PROJECT_EDIT_H

#include "Message.h"

class MessageProjectEdit: public Message<MessageProjectEdit>
{
public:
	MessageProjectEdit() {}

	static const std::string getStaticType()
	{
		return "MessageProjectEdit";
	}
};

#endif	  // MESSAGE_PROJECT_EDIT_H
