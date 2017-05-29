#ifndef MESSAGE_PROJECT_NEW_H
#define MESSAGE_PROJECT_NEW_H

#include "utility/messaging/Message.h"

class MessageProjectNew
	: public Message<MessageProjectNew>
{
public:
	MessageProjectNew(const std::string cdbPath, const std::vector<std::string> headerPaths)
		: cdbPath(cdbPath)
		, headerPaths(headerPaths)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageProjectNew";
	}

	const std::string cdbPath;
	const std::vector<std::string> headerPaths;
};

#endif // MESSAGE_PROJECT_NEW_H
