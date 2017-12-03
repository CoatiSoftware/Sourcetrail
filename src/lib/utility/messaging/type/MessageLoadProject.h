#ifndef MESSAGE_LOAD_PROJECT_H
#define MESSAGE_LOAD_PROJECT_H

#include "project/RefreshInfo.h"

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"

class MessageLoadProject
	: public Message<MessageLoadProject>
{
public:
	MessageLoadProject(const FilePath& filePath, bool settingsChanged = false, RefreshMode refreshMode = REFRESH_NONE)
		: projectSettingsFilePath(filePath)
		, settingsChanged(settingsChanged)
		, refreshMode(refreshMode)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLoadProject";
	}

	virtual void print(std::ostream& os) const
	{
		os << projectSettingsFilePath.str();
		os << ", settingsChanged: " << std::boolalpha << settingsChanged;
		os << ", refreshMode: " << refreshMode;
	}

	const FilePath projectSettingsFilePath;
	const bool settingsChanged;
	const RefreshMode refreshMode;
};

#endif // MESSAGE_LOAD_PROJECT_H
