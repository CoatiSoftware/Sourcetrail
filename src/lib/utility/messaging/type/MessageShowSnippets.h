#ifndef MESSAGE_SHOW_SNIPPETS_H
#define MESSAGE_SHOW_SNIPPETS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class TokenLocationFile;

class MessageShowSnippets
	: public Message<MessageShowSnippets>
{
public:
	MessageShowSnippets(std::shared_ptr<TokenLocationFile> locationFile)
		: locationFile(locationFile)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowSnippets";
	}

	std::shared_ptr<TokenLocationFile> locationFile;
};

#endif // MESSAGE_SHOW_SNIPPETS_H
