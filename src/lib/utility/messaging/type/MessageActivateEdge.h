#ifndef MESSAGE_ACTIVATE_EDGE_H
#define MESSAGE_ACTIVATE_EDGE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/graph/Edge.h"
#include "data/name/NameHierarchy.h"

class MessageActivateEdge
	: public Message<MessageActivateEdge>
{
public:
	MessageActivateEdge(Id tokenId, Edge::EdgeType type, const NameHierarchy& fromName, const NameHierarchy& toName)
		: tokenId(tokenId)
		, type(type)
		, fromNameHierarchy(fromName)
		, toNameHierarchy(toName)
	{
		setKeepContent(true);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateEdge";
	}

	bool isAggregation() const
	{
		return type == Edge::EDGE_AGGREGATION;
	}

	std::string getFullName() const
	{
		return Edge::getTypeString(type) + ":" + fromNameHierarchy.getFullName() + "->" + toNameHierarchy.getFullName();
	}

	virtual void print(std::ostream& os) const
	{
		os << tokenId << " - " << getFullName();
	}

	const Id tokenId;
	const Edge::EdgeType type;
	const NameHierarchy fromNameHierarchy;
	const NameHierarchy toNameHierarchy;
};

#endif // MESSAGE_ACTIVATE_EDGE_H
