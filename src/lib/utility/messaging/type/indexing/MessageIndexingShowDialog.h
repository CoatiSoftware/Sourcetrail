#ifndef MESSAGE_INDEXING_SHOW_DIALOG_H
#define MESSAGE_INDEXING_SHOW_DIALOG_H

#include "utility/messaging/Message.h"

class MessageIndexingShowDialog
	: public Message<MessageIndexingShowDialog>
{
public:
	static const std::string getStaticType()
	{
		return "MessageIndexingShowDialog";
	}

	MessageIndexingShowDialog(bool showDialog)
		: showDialog(showDialog)
	{
		setSendAsTask(false);
	}

	const bool showDialog;
};

#endif // MESSAGE_INDEXING_SHOW_DIALOG_H
