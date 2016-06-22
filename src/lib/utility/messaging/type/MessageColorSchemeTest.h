#ifndef MESSAGE_COLOR_SCHEME_TEST_H
#define MESSAGE_COLOR_SCHEME_TEST_H

#include "utility/messaging/Message.h"

class MessageColorSchemeTest
	: public Message<MessageColorSchemeTest>
{
public:
	MessageColorSchemeTest()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageColorSchemeTest";
	}
};

#endif // MESSAGE_COLOR_SCHEME_TEST_H
