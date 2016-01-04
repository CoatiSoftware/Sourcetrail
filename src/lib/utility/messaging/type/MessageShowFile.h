#ifndef MESSAGE_SHOW_FILE_H
#define MESSAGE_SHOW_FILE_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageShowFile
	: public Message<MessageShowFile>
{
public:
	MessageShowFile(const FilePath& filePath, bool showErrors)
		: filePath(filePath)
		, showErrors(showErrors)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowFile";
	}

	virtual void print(std::ostream& os) const
	{
		os << filePath.str();
	}

	const FilePath filePath;
	const bool showErrors;
};

#endif // MESSAGE_SHOW_FILE_H
