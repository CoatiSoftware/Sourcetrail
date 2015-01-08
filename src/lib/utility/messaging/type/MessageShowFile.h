#ifndef MESSAGE_SHOW_FILE_H
#define MESSAGE_SHOW_FILE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageShowFile: public Message<MessageShowFile>
{
public:
	MessageShowFile(const std::string& filePath, uint startLineNumber, uint endLineNumber)
		: filePath(filePath)
		, startLineNumber(startLineNumber)
		, endLineNumber(endLineNumber)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowFile";
	}

	const std::string filePath;
	const uint startLineNumber;
	const uint endLineNumber;
};

#endif // MESSAGE_SHOW_FILE_H
