#ifndef MESSAGE_SCROLL_TO_LINE_H
#define MESSAGE_SCROLL_TO_LINE_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"

class MessageScrollToLine
	: public Message<MessageScrollToLine>
{
public:
	MessageScrollToLine(const FilePath& filePath, size_t line)
		: filePath(filePath)
		, line(line)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageScrollToLine";
	}

	const FilePath filePath;
	size_t line;
};

#endif // MESSAGE_SCROLL_TO_LINE_H
