#ifndef MESSAGE_SEARCH_FULLTEXT_H
#define MESSAGE_SEARCH_FULLTEXT_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageSearchFullText: public Message<MessageSearchFullText>
{
public:
	MessageSearchFullText(const std::string& searchTerm)
		: searchTerm(searchTerm)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchFullText";
	}

	virtual void print(std::ostream& os) const
	{
		os << searchTerm;
	}

	const std::string searchTerm;
};

#endif // MESSAGE_SEARCH_FULLTEXT_H
