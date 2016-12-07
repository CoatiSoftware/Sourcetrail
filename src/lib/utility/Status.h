#ifndef STATUS_H
#define STATUS_H

#include <string>

struct Status
{
	Status(std::string message, bool isError = false)
		: message(message)
		, isError(isError){};

	std::string message;
	bool isError;
};

enum STATUSTYPE
{
	STATUS_INFO = 1,
	STATUS_ERROR = 2,
};

typedef int StatusFilter;

#endif //STATUS_H
