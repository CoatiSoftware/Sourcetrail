#ifndef MESSAGE_SEARCH_FULLTEXT_H
#define MESSAGE_SEARCH_FULLTEXT_H

#include "utility/messaging/Message.h"

class MessageSearchFullText: public Message<MessageSearchFullText>
{
public:
	MessageSearchFullText(const std::string& searchTerm, bool caseSensitive = false)
		: searchTerm(searchTerm)
		, caseSensitive(caseSensitive)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchFullText";
	}

	virtual void print(std::wostream& os) const
	{
		os << utility::decodeFromUtf8(searchTerm);
	}

	const std::string searchTerm;
	bool caseSensitive;
};

#endif // MESSAGE_SEARCH_FULLTEXT_H
