#ifndef MESSAGE_TO_UNDO_REDO_POSITION_H
#define MESSAGE_TO_UNDO_REDO_POSITION_H

#include "utility/messaging/Message.h"

class MessageToUndoRedoPosition
	: public Message<MessageToUndoRedoPosition>
{
public:
	MessageToUndoRedoPosition(size_t index)
		: index(index)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageToUndoRedoPosition";
	}

	size_t index;
};

#endif // MESSAGE_TO_UNDO_REDO_POSITION_H
