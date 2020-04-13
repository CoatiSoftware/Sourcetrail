#ifndef MESSAGE_INDEXING_SHOW_DIALOG_H
#define MESSAGE_INDEXING_SHOW_DIALOG_H

#include "../../Message.h"

class MessageIndexingShowDialog: public Message<MessageIndexingShowDialog>
{
public:
	static const std::string getStaticType()
	{
		return "MessageIndexingShowDialog";
	}

	MessageIndexingShowDialog()
	{
		setSendAsTask(false);
	}
};

#endif	  // MESSAGE_INDEXING_SHOW_DIALOG_H
