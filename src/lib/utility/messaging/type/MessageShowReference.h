#ifndef MESSAGE_SHOW_REFERENCE_H
#define MESSAGE_SHOW_REFERENCE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageShowReference
	: public Message<MessageShowReference>
{
public:
	MessageShowReference(size_t refIndex, Id tokenId, Id locationId)
		: refIndex(refIndex)
		, tokenId(tokenId)
		, locationId(locationId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowReference";
	}

	virtual void print(std::ostream& os) const
	{
		os << "index: " << refIndex << " token: " << tokenId << " location: " << locationId;
	}

	const size_t refIndex;
	const Id tokenId;
	const Id locationId;
};

#endif // MESSAGE_SHOW_REFERENCE_H
