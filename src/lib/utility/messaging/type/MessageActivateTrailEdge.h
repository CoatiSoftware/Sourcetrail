#ifndef MESSAGE_ACTIVATE_TRAIL_EDGE_H
#define MESSAGE_ACTIVATE_TRAIL_EDGE_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

#include "data/graph/Edge.h"
#include "data/name/NameHierarchy.h"

class MessageActivateTrailEdge
	: public Message<MessageActivateTrailEdge>
{
public:
	MessageActivateTrailEdge(
		Id tokenId, Edge::EdgeType type, const NameHierarchy& sourceNameHierarchy, const NameHierarchy& targetNameHierarchy
	)
		: tokenId(tokenId)
		, type(type)
		, sourceNameHierarchy(sourceNameHierarchy)
		, targetNameHierarchy(targetNameHierarchy)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTrailEdge";
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
};

#endif // MESSAGE_ACTIVATE_TRAIL_EDGE_H
