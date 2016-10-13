#ifndef MESSAGE_NEW_ERRORS_H
#define MESSAGE_NEW_ERRORS_H

#include "utility/messaging/Message.h"

#include "data/StorageTypes.h"

class MessageNewErrors
	: public Message<MessageNewErrors>
{
public:
	MessageNewErrors(const std::vector<StorageError>& errors)
		: errors(errors)
	{
		setSendAsTask(false);
	}

	static const std::string getStaticType()
	{
		return "MessageNewErrors";
	}

	virtual void print(std::ostream& os) const
	{
		os << errors.size() << " errors";
	}

	const std::vector<StorageError> errors;
};

#endif // MESSAGE_NEW_ERRORS_H
