#ifndef MESSAGE_SWITCH_COLOR_SCHEME_H
#define MESSAGE_SWITCH_COLOR_SCHEME_H

#include "utility/messaging/Message.h"

class MessageSwitchColorScheme
	: public Message<MessageSwitchColorScheme>
{
public:
	MessageSwitchColorScheme(const FilePath& filePath)
		: colorSchemePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSwitchColorScheme";
	}

	virtual void print(std::ostream& os) const
	{
		os << colorSchemePath.str();
	}

	const FilePath colorSchemePath;
};

#endif // MESSAGE_SWITCH_COLOR_SCHEME_H
