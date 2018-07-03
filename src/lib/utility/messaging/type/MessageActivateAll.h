#ifndef MESSAGE_ACTIVATE_ALL_H
#define MESSAGE_ACTIVATE_ALL_H

#include "utility/messaging/Message.h"
#include "utility/messaging/type/MessageActivateBase.h"

#include "data/NodeTypeSet.h"

class MessageActivateAll
	: public Message<MessageActivateAll>
	, public MessageActivateBase
{
public:
	static const std::string getStaticType()
	{
		return "MessageActivateAll";
	}

	MessageActivateAll(NodeTypeSet acceptedNodeTypes = NodeTypeSet::all())
		: acceptedNodeTypes(acceptedNodeTypes)
	{
		setIsParallel(true);
	}

	std::vector<SearchMatch> getSearchMatches() const override
	{
		if (acceptedNodeTypes != NodeTypeSet::all())
		{
			return SearchMatch::createCommandsForNodeTypes(acceptedNodeTypes);
		}
		return { SearchMatch::createCommand(SearchMatch::COMMAND_ALL) };
	}

	NodeTypeSet acceptedNodeTypes;
};

#endif // MESSAGE_ACTIVATE_ALL_H
