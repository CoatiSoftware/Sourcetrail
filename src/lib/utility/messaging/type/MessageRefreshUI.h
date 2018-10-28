#ifndef MESSAGE_REFRESH_UI_H
#define MESSAGE_REFRESH_UI_H

#include "Message.h"

class MessageRefreshUI
	: public Message<MessageRefreshUI>
{
public:
	static const std::string getStaticType()
	{
		return "MessageRefreshUI";
	}

	MessageRefreshUI()
		: loadStyle(true)
	{
	}

	MessageRefreshUI& noStyleReload()
	{
		loadStyle = false;
		return *this;
	}

	void print(std::wostream& os) const override
	{
		if (loadStyle)
		{
			os << "reload style";
		}
	}

	bool loadStyle;
};

#endif // MESSAGE_REFRESH_UI_H
