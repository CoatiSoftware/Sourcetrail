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
	MessageActivateEdge(Id tokenId, Edge::EdgeType type, const NameHierarchy& sourceNameHierarchy, const NameHierarchy& targetNameHierarchy)
		: tokenId(tokenId)
		, type(type)
		, sourceNameHierarchy(sourceNameHierarchy)
		, targetNameHierarchy(targetNameHierarchy)
	{
		if (!isAggregation())
		{
			setKeepContent(true);
		}
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
		std::string name = Edge::getReadableTypeString(type) + ":";
		name += sourceNameHierarchy.getQualifiedNameWithSignature() + "->";
		name += targetNameHierarchy.getQualifiedNameWithSignature();
		return name;
	}

	virtual void print(std::ostream& os) const
	{
		os << tokenId << " - " << getFullName();
	}

	const Id tokenId;
	const Edge::EdgeType type;
	const NameHierarchy sourceNameHierarchy;
	const NameHierarchy targetNameHierarchy;

	std::vector<Id> aggregationIds;
};

#endif // MESSAGE_ACTIVATE_EDGE_H
