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
		UndoType_Undo
	};

	MessageBase()
		: UndoRedoType(UndoType_Normal)
		, m_sendAsTask(true)
	{
	}

	virtual ~MessageBase()
	{
	}

	virtual std::string getType() const = 0;
	virtual void dispatch() = 0;

	bool sendAsTask() const
	{
		return m_sendAsTask;
	}

	void setSendAsTask(bool sendAsTask)
	{
		m_sendAsTask = sendAsTask;
	}

	UndoType UndoRedoType;

private:
	bool m_sendAsTask;
};

#endif // MESSAGE_BASE_H
