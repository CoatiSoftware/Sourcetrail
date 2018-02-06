#ifndef MESSAGE_ACTIVATE_LOCAL_SYMBOLS_H
#define MESSAGE_ACTIVATE_LOCAL_SYMBOLS_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageActivateLocalSymbols
	: public Message<MessageActivateLocalSymbols>
{
public:
	MessageActivateLocalSymbols()
	{
	}

	MessageActivateLocalSymbols(const std::vector<Id>& symbolIds)
		: symbolIds(symbolIds)
	{
	}

	void addSymbol(Id symbolId)
	{
		symbolIds.push_back(symbolId);
	}

	static const std::string getStaticType()
	{
		return "MessageActivateLocalSymbols";
	}

	virtual void print(std::wostream& os) const
	{
		for (const Id& symbolId : symbolIds)
		{
			os << symbolId << L" ";
		}
	}

	std::vector<Id> symbolIds;
};

#endif // MESSAGE_ACTIVATE_LOCAL_SYMBOLS_H
