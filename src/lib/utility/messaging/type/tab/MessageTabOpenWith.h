#ifndef MESSAGE_TAB_OPEN_WITH_H
#define MESSAGE_TAB_OPEN_WITH_H

#include "FilePath.h"
#include "Message.h"
#include "SearchMatch.h"

class MessageTabOpenWith: public Message<MessageTabOpenWith>
{
public:
	static const std::string getStaticType()
	{
		return "MessageTabOpenWith";
	}

	MessageTabOpenWith(Id tokenId, Id locationId = 0): tokenId(tokenId), locationId(locationId) {}

	MessageTabOpenWith(const FilePath& path, size_t line = 0): filePath(path), line(line) {}

	MessageTabOpenWith(const SearchMatch& match): match(match) {}

	MessageTabOpenWith& showNewTab(bool show)
	{
		showTab = show;
		return *this;
	}

	const Id tokenId = 0;
	const Id locationId = 0;

	const FilePath filePath;
	const SearchMatch match;

	const size_t line = 0;

	bool showTab = false;
};

#endif	  // MESSAGE_TAB_OPEN_WITH_H
