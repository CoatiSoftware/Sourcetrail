#ifndef MESSAGE_TO_NEXT_CODE_REFERENCE_H
#define MESSAGE_TO_NEXT_CODE_REFERENCE_H

#include "Message.h"
#include "TabId.h"

class MessageToNextCodeReference: public Message<MessageToNextCodeReference>
{
public:
	MessageToNextCodeReference(const FilePath& filePath, size_t lineNumber, bool next)
		: filePath(filePath), lineNumber(lineNumber), next(next)
	{
		setSchedulerId(TabId::currentTab());
	}

	static const std::string getStaticType()
	{
		return "MessageToNextCodeReference";
	}

	virtual void print(std::wostream& os) const
	{
		os << filePath.wstr() << L' ' << lineNumber << L' ';

		if (next)
		{
			os << L"next";
		}
		else
		{
			os << L"previous";
		}
	}

	const FilePath filePath;
	const size_t lineNumber;
	const bool next;
};

#endif	  // MESSAGE_TO_NEXT_CODE_REFERENCE_H
