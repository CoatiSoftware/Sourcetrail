#ifndef MESSAGE_SHOW_ERROR_H
#define MESSAGE_SHOW_ERROR_H

#include "../../Message.h"
#include "../../../../component/TabId.h"

class MessageShowError: public Message<MessageShowError>
{
public:
	static const std::string getStaticType()
	{
		return "MessageShowError";
	}

	MessageShowError(Id errorId): errorId(errorId)
	{
		setSchedulerId(TabId::currentTab());
	}

	const Id errorId;
};

#endif	  // MESSAGE_SHOW_ERROR_H
