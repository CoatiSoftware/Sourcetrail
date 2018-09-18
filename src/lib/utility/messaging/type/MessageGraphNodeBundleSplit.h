#ifndef MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H
#define MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H

#include "Message.h"
#include "types.h"

class MessageGraphNodeBundleSplit
	: public Message<MessageGraphNodeBundleSplit>
{
public:
	MessageGraphNodeBundleSplit(Id bundleId, bool removeOtherNodes = false, bool layoutToList = false)
		: bundleId(bundleId)
		, removeOtherNodes(removeOtherNodes)
		, layoutToList(layoutToList)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageGraphNodeBundleSplit";
	}

	virtual void print(std::wostream& os) const
	{
		os << bundleId;
	}

	Id bundleId;
	bool removeOtherNodes;
	bool layoutToList;
};

#endif // MESSAGE_GRAPH_NODE_BUNDLE_SPLIT_H
