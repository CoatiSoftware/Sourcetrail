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

	virtual void print(std::ostream& os) const
	{
		os << tokenId << " ";
		if (expand)
		{
			os << "expand";
		}
		else
		{
			os << "collapse";
		}
	}

	const Id tokenId;
	const bool expand;
	const bool ignoreIfNotReplayed;
};

#endif // MESSAGE_GRAPH_NODE_EXPAND_H
