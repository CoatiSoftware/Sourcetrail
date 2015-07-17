#ifndef MESSAGE_SWITCH_COLOR_SCHEME_H
#define MESSAGE_SWITCH_COLOR_SCHEME_H

#include "utility/messaging/Message.h"

class MessageSwitchColorScheme
	: public Message<MessageSwitchColorScheme>
{
public:
	MessageSwitchColorScheme(const std::string& filePath)
		: colorSchemeFilePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSwitchColorScheme";
	}

	const std::string colorSchemeFilePath;
};

#endif // MESSAGE_SWITCH_COLOR_SCHEME_H
