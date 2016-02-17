#ifndef MESSAGE_PROJECT_NEW_H
#define MESSAGE_PROJECT_NEW_H

#include "utility/messaging/Message.h"

class MessageProjectNew
	: public Message<MessageProjectNew>
{
public:
	MessageProjectNew()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageProjectNew";
	}

	bool fromVisualStudioSolution() const
	{
		return visualStudioSolutionPath.size() > 0;
	}

	void setVisualStudioSolutionPath(const std::string& path)
	{
		visualStudioSolutionPath = path;
	}

	std::string visualStudioSolutionPath;
};

#endif // MESSAGE_PROJECT_NEW_H
