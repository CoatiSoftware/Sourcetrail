#ifndef MESSAGE_IDE_CREATE_CDB_H
#define MESSAGE_IDE_CREATE_CDB_H

#include "utility/messaging/Message.h"

class MessageIDECreateCDB : public Message<MessageIDECreateCDB>
{
public:
	MessageIDECreateCDB()
	{
	}

	static const std::string getStaticType()
	{
		return "MessageIDECreateCDB";
	}

	virtual void print(std::ostream& os) const
	{
		os << "Create CDB from current solution";
	}
};

#endif // MESSAGE_IDE_CREATE_CDB_H