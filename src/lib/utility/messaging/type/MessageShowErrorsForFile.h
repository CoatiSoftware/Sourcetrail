#ifndef MESSAGE_SHOW_ERRORS_FOR_FILE_H
#define MESSAGE_SHOW_ERRORS_FOR_FILE_H

#include "utility/file/FilePath.h"
#include "utility/messaging/Message.h"

class MessageShowErrorsForFile
	: public Message<MessageShowErrorsForFile>
{
public:
	MessageShowErrorsForFile(const FilePath& filePath)
		: filePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageShowErrorsForFile";
	}

	const FilePath filePath;
};

#endif // MESSAGE_SHOW_ERRORS_FOR_FILE_H
