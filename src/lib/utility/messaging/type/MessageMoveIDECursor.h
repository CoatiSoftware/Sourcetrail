#ifndef MESSAGE_MOVE_IDE_CURSOR_H
#define MESSAGE_MOVE_IDE_CURSOR_H

#include "utility/messaging/Message.h"

class MessageMoveIDECursor : public Message<MessageMoveIDECursor>
{
public:
	MessageMoveIDECursor(const std::string& FilePos, const unsigned int Row, const unsigned int Column)
		: FilePosition(FilePos)
		, Row(Row)
		, Column(Column)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageMoveIDECursor";
	}

	virtual void print(std::ostream& os) const
	{
		os << FilePosition << ":" << Row << ":" << Column;
	}

	const std::string FilePosition;
	const unsigned int Row;
	const unsigned int Column;
};

#endif // MESSAGE_MOVE_IDE_CURSOR_H