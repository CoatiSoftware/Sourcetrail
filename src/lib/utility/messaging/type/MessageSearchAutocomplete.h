#ifndef MESSAGE_SEARCH_AUTOCOMPLETE_H
#define MESSAGE_SEARCH_AUTOCOMPLETE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageSearchAutocomplete: public Message<MessageSearchAutocomplete>
{
public:
	MessageSearchAutocomplete(const std::string& query, const std::string& word)
		: query(query)
		, word(word)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchAutocomplete";
	}

	const std::string query;
	const std::string word;
};

#endif // MESSAGE_SEARCH_AUTOCOMPLETE_H
