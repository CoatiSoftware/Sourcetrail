#ifndef MESSAGE_LOAD_SOURCE_H
#define MESSAGE_LOAD_SOURCE_H

#include "utility/messaging/Message.h"

class MessageLoadSource: public Message<MessageLoadSource>
{
public:
	MessageLoadSource(const std::string& sourceDir)
		: sourceDirectoryPath(sourceDir)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLoadSource";
	}

	const std::string sourceDirectoryPath;
};

#endif // MESSAGE_LOAD_SOURCE_H
