#ifndef MESSAGE_SHOW_FILE_H
#define MESSAGE_SHOW_FILE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageShowFile: public Message<MessageShowFile>
{
public:
	MessageShowFile(const std::string& filePath, uint lineNumber, const std::vector<Id>& activeTokenIds)
		: filePath(filePath)
		, lineNumber(lineNumber)
		, activeTokenIds(activeTokenIds)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowFile";
	}

	const std::string filePath;
	const uint lineNumber;
	const std::vector<Id> activeTokenIds;
};

#endif // MESSAGE_SHOW_FILE_H
