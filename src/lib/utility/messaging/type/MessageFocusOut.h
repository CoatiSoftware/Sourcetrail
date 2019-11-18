#ifndef MESSAGE_FOCUS_OUT_H
#define MESSAGE_FOCUS_OUT_H

#include <vector>

#include "Message.h"
#include "TabId.h"
#include "types.h"

class MessageFocusOut: public Message<MessageFocusOut>
{
public:
	MessageFocusOut(const std::vector<Id>& tokenIds): tokenIds(tokenIds)
	{
		setIsLogged(false);
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageFocusOut";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id: tokenIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> tokenIds;
};

#endif	  // MESSAGE_FOCUS_OUT_H
