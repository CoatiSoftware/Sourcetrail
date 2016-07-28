#ifndef MESSAGE_SCROLL_TO_LINE_h
#define MESSAGE_SCROLL_TO_LINE_h

#include "utility/messaging/Message.h"

class MessageScrollToLine
	: public Message<MessageScrollToLine>
{
public:
	MessageScrollToLine(std::string filename, unsigned int line, bool isModified = false)
		: filename(filename)
		, line(line)
		, isModified(isModified)
	{
		setIsLogged(false);
	}

	static const std::string getStaticType()
	{
		return "MessageScrollToLine";
	}

	std::string filename;
	unsigned int line;
	bool isModified;
};

#endif // MESSAGE_SCROLL_TO_LINE_h
