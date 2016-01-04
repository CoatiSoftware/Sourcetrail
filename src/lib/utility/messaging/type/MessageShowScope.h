#ifndef MESSAGE_SHOW_SCOPE_H
#define MESSAGE_SHOW_SCOPE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageShowScope
	: public Message<MessageShowScope>
{
public:
	MessageShowScope(Id scopeLocationId)
		: scopeLocationId(scopeLocationId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowScope";
	}

	virtual void print(std::ostream& os) const
	{
		os << scopeLocationId;
	}

	const Id scopeLocationId;
};

#endif // MESSAGE_SHOW_SCOPE_H
