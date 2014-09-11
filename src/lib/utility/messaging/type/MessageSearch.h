#ifndef MESSAGE_SEARCH_H
#define MESSAGE_SEARCH_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageSearch: public Message<MessageSearch>
{
public:
	MessageSearch(const std::string& query)
		: query(query)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearch";
	}

	const std::string query;
};

#endif // MESSAGE_SEARCH_H
