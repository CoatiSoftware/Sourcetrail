#ifndef MESSAGE_SHOW_REFERENCE_H
#define MESSAGE_SHOW_REFERENCE_H

#include "Message.h"
#include "TabId.h"
#include "types.h"

class MessageShowReference
	: public Message<MessageShowReference>
{
public:
	MessageShowReference(size_t refIndex, Id tokenId, Id locationId, bool fromUser)
		: refIndex(refIndex)
		, tokenId(tokenId)
		, locationId(locationId)
		, fromUser(fromUser)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageShowReference";
	}

	virtual void print(std::wostream& os) const
	{
		os << L"index: " << refIndex << L" token: " << tokenId << L" location: " << locationId;
	}

	const size_t refIndex;
	const Id tokenId;
	const Id locationId;
	const bool fromUser;
};

#endif // MESSAGE_SHOW_REFERENCE_H
