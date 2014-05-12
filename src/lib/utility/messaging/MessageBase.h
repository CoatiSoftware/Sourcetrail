#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

#include <string>

class MessageBase
{
public:
	virtual ~MessageBase()
	{
	}

	virtual std::string getType() const = 0;
};

#endif // MESSAGE_BASE_H
