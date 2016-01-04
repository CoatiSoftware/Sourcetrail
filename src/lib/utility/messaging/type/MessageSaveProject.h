#ifndef MESSAGE_SAVE_PROJECT_H
#define MESSAGE_SAVE_PROJECT_H

#include "utility/messaging/Message.h"

class MessageSaveProject: public Message<MessageSaveProject>
{
public:
	MessageSaveProject(const std::string& filePath)
		: projectSettingsFilePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSaveProject";
	}

	virtual void print(std::ostream& os) const
	{
		os << projectSettingsFilePath;
	}

	const std::string projectSettingsFilePath;
};

#endif // MESSAGE_SAVE_PROJECT_H
