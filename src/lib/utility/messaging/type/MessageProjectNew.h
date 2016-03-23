#ifndef MESSAGE_PROJECT_NEW_H
#define MESSAGE_PROJECT_NEW_H

#include "utility/messaging/Message.h"

class MessageProjectNew
	: public Message<MessageProjectNew>
{
public:
	MessageProjectNew()
		: solutionPath("")
		, ideId("")
	{
	}

	static const std::string getStaticType()
	{
		return "MessageProjectNew";
	}

	bool fromSolution() const
	{
		return solutionPath.size() > 0;
	}

	void setSolutionPath(const std::string& path)
	{
		solutionPath = path;
	}

	std::string solutionPath;
	std::string ideId;
};

#endif // MESSAGE_PROJECT_NEW_H
