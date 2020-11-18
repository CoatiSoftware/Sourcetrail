#ifndef MESSAGE_SAVE_AS_IMAGE_H
#define MESSAGE_SAVE_AS_IMAGE_H

#include "Message.h"


class MessageSaveAsImage: public Message<MessageSaveAsImage>
{
public:
	MessageSaveAsImage(QString path) : path(path) {}

	static const std::string getStaticType()
	{
		return "MessageSaveAsImage";
	}

	QString path;
};

#endif /* MESSAGE_SAVE_AS_IMAGE_H */
