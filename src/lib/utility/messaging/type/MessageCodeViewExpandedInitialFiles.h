#ifndef MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H
#define MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H

#include "utility/messaging/Message.h"

class MessageCodeViewExpandedInitialFiles
	: public Message<MessageCodeViewExpandedInitialFiles>
{
public:
	MessageCodeViewExpandedInitialFiles(bool scrollToDefinition)
		: scrollToDefinition(scrollToDefinition)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageCodeViewExpandedInitialFiles";
	}

	bool scrollToDefinition;
};

#endif // MESSAGE_CODE_VIEW_EXPANDED_INITIAL_FILES_H
