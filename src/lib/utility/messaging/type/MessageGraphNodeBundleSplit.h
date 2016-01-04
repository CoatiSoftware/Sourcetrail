#ifndef MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H
#define MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageGraphNodeBundleSplit
	: public Message<MessageGraphNodeBundleSplit>
{
public:
	MessageGraphNodeBundleSplit(Id bundleId)
		: bundleId(bundleId)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeBundleSplit";
	}

	virtual void print(std::ostream& os) const
	{
		os << bundleId;
	}

	Id bundleId;
};

#endif // MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H
