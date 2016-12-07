#ifndef MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H
#define MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H

#include "utility/messaging/Message.h"

class MessageCodeViewExpandedInitialFiles
	: public Message<MessageCodeViewExpandedInitialFiles>
{
public:
	MessageCodeViewExpandedInitialFiles()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCodeViewExpandedInitialFiles";
	}
};

#endif // MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H
