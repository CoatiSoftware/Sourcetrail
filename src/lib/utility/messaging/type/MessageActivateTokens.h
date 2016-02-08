#ifndef MESSAGE_ACTIVATE_TOKENS_H
#define MESSAGE_ACTIVATE_TOKENS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateTokens
	: public Message<MessageActivateTokens>
{
public:
	MessageActivateTokens(const MessageBase* other, const std::vector<Id>& tokenIds)
		: tokenIds(tokenIds)
		, isEdge(false)
		, isAggregation(false)
		, isFromSystem(false)
		, isFromSearch(false)
	{
		undoRedoType = other->undoRedoType;
		setKeepContent(other->keepContent());
		setIsLast(other->isLast());
	}

	static const std::string getStaticType()
	{
		return "MessageActivateTokens";
	}

	virtual void print(std::ostream& os) const
	{
		for (const Id& id : tokenIds)
		{
			os << id << " ";
		}
	}

	const std::vector<Id> tokenIds;
	std::vector<Id> originalTokenIds;

	bool isEdge;
	bool isAggregation;
	bool isFromSystem;
	bool isFromSearch;
};

#endif // MESSAGE_ACTIVATE_TOKENS_H
