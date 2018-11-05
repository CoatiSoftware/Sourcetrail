#ifndef MESSAGE_ACTIVATE_TOKEN_IDS_H
#define MESSAGE_ACTIVATE_TOKEN_IDS_H

#include "Message.h"
#include "TabId.h"
#include "types.h"

class MessageActivateTokenIds
	: public Message<MessageActivateTokenIds>
{
public:
	MessageActivateTokenIds(const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokenIds";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << L" ";
		}
	}

	const std::vector<Id> tokenIds;
};

#endif // MESSAGE_ACTIVATE_TOKEN_IDS_H
