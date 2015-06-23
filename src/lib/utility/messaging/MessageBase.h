#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <string>

class MessageBase
{
public:
	enum UndoType
	{
		UNDOTYPE_NORMAL,
		UNDOTYPE_REDO,
		UNDOTYPE_UNDO,
		UNDOTYPE_IGNORE
	};

	MessageBase()
		: undoRedoType(UNDOTYPE_NORMAL)
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

	bool isFresh() const
	{
		return (undoRedoType == UNDOTYPE_NORMAL);
	}

	bool isIgnorable() const
	{
		return (undoRedoType == UNDOTYPE_IGNORE);
	}

	UndoType undoRedoType;

private:
	bool m_sendAsTask;
};

#endif // MESSAGE_BASE_H
