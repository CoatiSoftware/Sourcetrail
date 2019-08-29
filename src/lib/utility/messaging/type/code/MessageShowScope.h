#ifndef MESSAGE_SHOW_SCOPE_H
#define MESSAGE_SHOW_SCOPE_H

#include "Message.h"
#include "types.h"
#include "TabId.h"

class MessageShowScope
	: public Message<MessageShowScope>
{
public:
	MessageShowScope(Id scopeLocationId, bool showErrors)
		: scopeLocationId(scopeLocationId)
		, showErrors(showErrors)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageShowScope";
	}

	virtual void print(std::wostream& os) const
	{
		os << scopeLocationId;
	}

	const Id scopeLocationId;
	const bool showErrors;
};

#endif // MESSAGE_SHOW_SCOPE_H
