#ifndef MESSAGE_ACTIVATE_ERRORS_H
#define MESSAGE_ACTIVATE_ERRORS_H

#include "utility/messaging/Message.h"
#include "utility/messaging/type/MessageActivateBase.h"

#include "data/ErrorFilter.h"

class MessageActivateErrors
	: public Message<MessageActivateErrors>
	, public MessageActivateBase
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateErrors";
	}

	MessageActivateErrors(const ErrorFilter& filter, const FilePath& file = FilePath())
		: filter(filter)
		, file(file)
	{
	}

	std::vector<SearchMatch> getSearchMatches() const override
	{
		std::vector<SearchMatch> matches = { SearchMatch::createCommand(SearchMatch::COMMAND_ERROR) };
		if (!file.empty())
		{
			SearchMatch match;
			match.name = match.text = file.fileName();
			match.searchType = SearchMatch::SEARCH_TOKEN;
			match.nodeType = NodeType::NODE_FILE;
			matches.push_back(match);
		}
		return matches;
	}

	const ErrorFilter filter;
	const FilePath file;
};

#endif // MESSAGE_ACTIVATE_ERRORS_H
