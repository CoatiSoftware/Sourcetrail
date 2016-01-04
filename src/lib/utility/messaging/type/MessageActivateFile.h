#ifndef MESSAGE_ACTIVATE_FILE_H
#define MESSAGE_ACTIVATE_FILE_H

#include "utility/messaging/Message.h"
#include "utility/file/FilePath.h"

class MessageActivateFile: public Message<MessageActivateFile>
{
public:
	MessageActivateFile(const FilePath& filePath)
		: filePath(filePath)
	{
	}

	static const std::string getStaticType()
	{
		return "MessageActivateFile";
	}

	virtual void print(std::ostream& os) const
	{
		os << filePath.str();
	}

	const FilePath filePath;
};

#endif // MESSAGE_ACTIVATE_FILE_H
