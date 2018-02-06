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

	virtual void print(std::wostream& os) const
	{
		os << utility::decodeFromUtf8(query) << L"[";
		std::vector<Id> nodeTypeIds = acceptedNodeTypes.getNodeTypeIds();
		for (size_t i = 0; i < nodeTypeIds.size(); i++)
		{
			if (i != 0)
			{
				os << L", ";
			}
			os << std::to_wstring(nodeTypeIds[i]);
		}
		os << L"]";
	}

	const std::string query;
	const NodeTypeSet acceptedNodeTypes;
};

#endif // MESSAGE_SEARCH_AUTOCOMPLETE_H
