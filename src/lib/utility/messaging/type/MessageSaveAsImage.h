#pragma once

#include "Message.h"


class MessageSaveAsImage: public Message<MessageSaveAsImage>
{
public:
	MessageSaveAsImage(QString path)
	{
		this->path = path;
	}

	static const std::string getStaticType()
	{
		return "MessageSaveAsImage";
	}

	QString path;
};
