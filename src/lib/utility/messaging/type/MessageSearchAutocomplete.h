#ifndef MESSAGE_SEARCH_AUTOCOMPLETE_H
#define MESSAGE_SEARCH_AUTOCOMPLETE_H

#include "data/graph/Node.h"
#include "data/NodeTypeSet.h"
#include "utility/messaging/Message.h"

class MessageSearchAutocomplete
	: public Message<MessageSearchAutocomplete>
{
public:
	MessageSearchAutocomplete(const std::string& query, NodeTypeSet acceptedNodeTypes)
		: query(query)
		, acceptedNodeTypes(acceptedNodeTypes)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageSearchAutocomplete";
	}

	virtual void print(std::ostream& os) const
	{
		os << query << "[";
		std::vector<Id> nodeTypeIds = acceptedNodeTypes.getNodeTypeIds();
		for (size_t i = 0; i < nodeTypeIds.size(); i++)
		{
			if (i != 0)
			{
				os << ", ";
			}
			os << std::to_string(nodeTypeIds[i]);
		}
		os << "]";
	}

	const std::string query;
	const NodeTypeSet acceptedNodeTypes;
};

#endif // MESSAGE_SEARCH_AUTOCOMPLETE_H
