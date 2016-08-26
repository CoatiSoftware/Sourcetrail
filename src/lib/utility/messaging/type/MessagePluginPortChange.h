#ifndef MESSAGE_PLUGIN_PORT_CHANGE_H
#define MESSAGE_PLUGIN_PORT_CHANGE_H

#include "utility/messaging/Message.h"

class MessagePluginPortChange
	: public Message<MessagePluginPortChange>
{
public:
	MessagePluginPortChange()
	{
	}

	static const std::string getStaticType()
	{
		return "MessagePluginPortChange";
	}
};

#endif // MESSAGE_PLUGIN_PORT_CHANGE_H
