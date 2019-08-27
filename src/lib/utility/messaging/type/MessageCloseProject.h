#ifndef MESSAGE_CLOSE_PROJECT_H
#define MESSAGE_CLOSE_PROJECT_H

#include "RefreshInfo.h"

#include "FilePath.h"
#include "Message.h"

class MessageCloseProject
	: public Message<MessageCloseProject>
{
public:
	MessageCloseProject()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCloseProject";
	}

	virtual void print(std::wostream& os) const
	{
	}
};

#endif // MESSAGE_CLOSE_PROJECT_H
