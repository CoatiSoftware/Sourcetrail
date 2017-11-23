#ifndef MESSAGE_SEARCH_AUTOCOMPLETE_H
#define MESSAGE_SEARCH_AUTOCOMPLETE_H

#include "data/graph/Node.h"
#include "utility/messaging/Message.h"

class MessageSearchAutocomplete
	: public Message<MessageSearchAutocomplete>
{
public:
	MessageSearchAutocomplete(const std::string& query, NodeType::TypeMask filter)
		: query(query)
		, filter(filter)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchAutocomplete";
	}

	virtual void print(std::ostream& os) const
	{
		os << query << " " << filter;
	}

	const std::string query;
	const NodeType::TypeMask filter;
};

#endif // MESSAGE_SEARCH_AUTOCOMPLETE_H
