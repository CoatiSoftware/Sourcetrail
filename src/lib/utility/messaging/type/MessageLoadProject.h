#ifndef MESSAGE_LOAD_PROJECT_H
#define MESSAGE_LOAD_PROJECT_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"

class MessageLoadProject
	: public Message<MessageLoadProject>
{
public:
	MessageLoadProject(const FilePath& filePath, bool forceRefresh)
		: projectSettingsFilePath(filePath)
		, forceRefresh(forceRefresh)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLoadProject";
	}

	virtual void print(std::ostream& os) const
	{
		os << projectSettingsFilePath.str() << ", forceRefresh: " << std::boolalpha << forceRefresh;
	}

	const FilePath projectSettingsFilePath;
	const bool forceRefresh;
};

#endif // MESSAGE_LOAD_PROJECT_H
