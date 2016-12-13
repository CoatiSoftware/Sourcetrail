#ifndef MESSAGE_REFRESH_H
#define MESSAGE_REFRESH_H

#include "utility/messaging/Message.h"

class MessageRefresh
	: public Message<MessageRefresh>
{
public:
	MessageRefresh()
		: uiOnly(false)
		, all(false)
		, loadStyle(true)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageRefresh";
	}

	MessageRefresh& refreshUiOnly()
	{
		uiOnly = true;
		return *this;
	}

	MessageRefresh& refreshAll()
	{
		all = true;
		return *this;
	}

	MessageRefresh& noReloadStyle()
	{
		loadStyle = false;
		return *this;
	}

	virtual void print(std::ostream& os) const
	{
		if (uiOnly)
		{
			os << "ui ";
		}
		if (all)
		{
			os << "all";
		}
	}

	bool uiOnly;
	bool all;

	bool loadStyle;
};

#endif // MESSAGE_REFRESH_H
