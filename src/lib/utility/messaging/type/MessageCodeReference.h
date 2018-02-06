#ifndef MESSAGE_CODE_REFERENCE_H
#define MESSAGE_CODE_REFERENCE_H

#include "utility/messaging/Message.h"

class MessageCodeReference
	: public Message<MessageCodeReference>
{
public:
	enum ReferenceType
	{
		REFERENCE_PREVIOUS,
		REFERENCE_NEXT
	};

	MessageCodeReference(ReferenceType type)
		: type(type)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCodeReference";
	}

	virtual void print(std::wostream& os) const
	{
		if (type == REFERENCE_PREVIOUS)
		{
			os << L"previous";
		}
		else
		{
			os << L"next";
		}
	}

	const ReferenceType type;
};

#endif // MESSAGE_CODE_REFERENCE_H
