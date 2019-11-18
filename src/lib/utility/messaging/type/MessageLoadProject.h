#ifndef MESSAGE_LOAD_PROJECT_H
#define MESSAGE_LOAD_PROJECT_H

#include "RefreshInfo.h"

#include "FilePath.h"
#include "Message.h"

class MessageLoadProject: public Message<MessageLoadProject>
{
public:
	MessageLoadProject(
		const FilePath& filePath, bool settingsChanged = false, RefreshMode refreshMode = REFRESH_NONE)
		: projectSettingsFilePath(filePath)
		, settingsChanged(settingsChanged)
		, refreshMode(refreshMode)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageLoadProject";
	}

	virtual void print(std::wostream& os) const
	{
		os << projectSettingsFilePath.wstr();
		os << L", settingsChanged: " << std::boolalpha << settingsChanged;
		os << L", refreshMode: " << refreshMode;
	}

	const FilePath projectSettingsFilePath;
	const bool settingsChanged;
	const RefreshMode refreshMode;
};

#endif	  // MESSAGE_LOAD_PROJECT_H
