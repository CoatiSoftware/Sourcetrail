#ifndef MESSAGE_ACTIVATE_ALL_H
#define MESSAGE_ACTIVATE_ALL_H

#include "Message.h"
#include "MessageActivateBase.h"

#include "NodeTypeSet.h"
#include "TabId.h"

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
		setSchedulerId(TabId::currentTab());
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
