#ifndef MESSAGE_FIND_H
#define MESSAGE_FIND_H

#include "../../Message.h"
#include "../../../../component/TabId.h"

class MessageFind: public Message<MessageFind>
{
public:
	MessageFind(bool fulltext = false): findFulltext(fulltext)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFind";
	}

	bool findFulltext;
};

#endif	  // MESSAGE_FIND_H
