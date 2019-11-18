#ifndef MESSAGE_SWITCH_COLOR_SCHEME_H
#define MESSAGE_SWITCH_COLOR_SCHEME_H

#include "Message.h"

class MessageSwitchColorScheme: public Message<MessageSwitchColorScheme>
{
public:
	MessageSwitchColorScheme(const FilePath& filePath): colorSchemePath(filePath) {}

	static const std::string getStaticType()
	{
		return "MessageSwitchColorScheme";
	}

	virtual void print(std::wostream& os) const
	{
		os << colorSchemePath.wstr();
	}

	const FilePath colorSchemePath;
};

#endif	  // MESSAGE_SWITCH_COLOR_SCHEME_H
