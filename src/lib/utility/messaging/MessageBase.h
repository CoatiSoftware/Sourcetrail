#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <ostream>
#include <sstream>

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
		, m_keepContent(false)
		, m_cancelled(false)
		, m_isLast(true)
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

	bool isLast() const
	{
		return m_isLast;
	}

	void setIsLast(bool isLast)
	{
		m_isLast = isLast;
	}

	void setKeepContent(bool keepContent)
	{
		m_keepContent = keepContent;
	}

	bool keepContent() const
	{
		return m_keepContent;
	}

	void cancel()
	{
		m_cancelled = true;
	}

	bool cancelled()
	{
		return m_cancelled;
	}

	virtual void print(std::ostream& os) const = 0;

	std::string str() const
	{
		std::stringstream ss;
		ss << getType() << " ";
		print(ss);
		return ss.str();
	}

	UndoType undoRedoType;

private:
	bool m_sendAsTask;
	bool m_keepContent;
	bool m_cancelled;
	bool m_isLast;
};

#endif // MESSAGE_BASE_H
