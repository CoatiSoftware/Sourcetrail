#ifndef MESSAGE_SEARCH_AUTOCOMPLETE_H
#define MESSAGE_SEARCH_AUTOCOMPLETE_H

#include "utility/messaging/Message.h"

class MessageSearchAutocomplete: public Message<MessageSearchAutocomplete>
{
public:
	MessageSearchAutocomplete(const std::string& query)
		: query(query)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchAutocomplete";
	}

	virtual void print(std::ostream& os) const
	{
		os << query;
	}

	const std::string query;
};

#endif // MESSAGE_SEARCH_AUTOCOMPLETE_H
