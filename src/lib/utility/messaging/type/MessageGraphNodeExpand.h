#ifndef MESSAGE_GRAPH_NODE_EXPAND_H
#define MESSAGE_GRAPH_NODE_EXPAND_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeExpand
	: public Message<MessageGraphNodeExpand>
{
public:
	MessageGraphNodeExpand(Id tokenId, bool expand, bool ignoreIfNotReplayed = false)
		: tokenId(tokenId)
		, expand(expand)
		, ignoreIfNotReplayed(ignoreIfNotReplayed)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeExpand";
	}

	virtual void print(std::wostream& os) const
	{
		os << tokenId << L" ";
		if (expand)
		{
			os << L"expand";
		}
		else
		{
			os << L"collapse";
		}
	}

	const Id tokenId;
	const bool expand;
	const bool ignoreIfNotReplayed;
};

#endif // MESSAGE_GRAPH_NODE_EXPAND_H
