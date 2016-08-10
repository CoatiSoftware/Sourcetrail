#ifndef MESSAGE_SCROLL_TO_LINE_h
#define MESSAGE_SCROLL_TO_LINE_h

#include "utility/messaging/Message.h"
#include "utility/file/FilePath.h"

class MessageScrollToLine
	: public Message<MessageScrollToLine>
{
public:
	MessageScrollToLine(const FilePath& filePath, unsigned int line, bool isModified = false)
		: filePath(filePath)
		, line(line)
		, isModified(isModified)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageScrollToLine";
	}

	const FilePath filePath;
	unsigned int line;
	bool isModified;
};

#endif // MESSAGE_SCROLL_TO_LINE_h
