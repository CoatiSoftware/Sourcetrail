#ifndef MESSAGE_PROJECT_NEW_H
#define MESSAGE_PROJECT_NEW_H

#include "utility/messaging/Message.h"

class MessageProjectNew
	: public Message<MessageProjectNew>
{
public:
	MessageProjectNew()
		: solutionPath("")
		, headerPaths()
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

	bool fromCDB() const
	{
		if (solutionPath.length() > 0)
		{
			size_t pos = solutionPath.find_last_of('.');
			if (pos != std::string::npos)
			{
				std::string extension = solutionPath.substr(pos + 1);

				return (extension == "json");
			}
			else
			{
				return false;
			}
		}

		return false;
	}

	void setSolutionPath(const std::string& path)
	{
		solutionPath = path;
	}

	void setHeaderPaths(const std::vector<std::string>& headerPaths)
	{
		this->headerPaths = headerPaths;
	}

	std::string solutionPath;
	std::vector<std::string> headerPaths;
	std::string ideId;
};

#endif // MESSAGE_PROJECT_NEW_H
