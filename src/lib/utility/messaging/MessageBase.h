#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <string>

class MessageBase
{
public:
	enum UndoType
	{
		UndoType_Normal,
		UndoType_Redo,
		UndoType_Undo,
	};
	MessageBase() : UndoRedoType(UndoType_Normal){};
	virtual ~MessageBase()
	{
	}

	virtual std::string getType() const = 0;
	virtual void dispatch() = 0;

	UndoType UndoRedoType;

};

#endif // MESSAGE_BASE_H
