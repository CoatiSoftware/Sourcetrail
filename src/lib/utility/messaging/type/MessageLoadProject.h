#ifndef MESSAGE_LOAD_PROJECT_H
#define MESSAGE_LOAD_PROJECT_H

#include "utility/messaging/Message.h"

class MessageLoadProject: public Message<MessageLoadProject>
{
public:
	MessageLoadProject(const std::string& filePath)
		: projectSettingsFilePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLoadProject";
	}

	virtual void print(std::ostream& os) const
	{
		os << projectSettingsFilePath;
	}

	const std::string projectSettingsFilePath;
};

#endif // MESSAGE_LOAD_PROJECT_H
