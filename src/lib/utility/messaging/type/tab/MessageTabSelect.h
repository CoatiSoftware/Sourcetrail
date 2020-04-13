#ifndef MESSAGE_TAB_SELECT_H
#define MESSAGE_TAB_SELECT_H

#include "../../Message.h"

class MessageTabSelect: public Message<MessageTabSelect>
{
public:
	MessageTabSelect(bool next): next(next) {}

	static const std::string getStaticType()
	{
		return "MessageTabSelect";
	}

	bool next;
};

#endif	  // MESSAGE_TAB_SELECT_H
