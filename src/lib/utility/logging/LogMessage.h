#ifndef LOG_MESSAGE_H
#define LOG_MESSAGE_H

#include <ctime>
#include <string>

struct LogMessage
{
public:
	LogMessage(
		const std::string& message,
		const std::string& fileName,
		const std::string& functionName,
		const unsigned int line,
		const std::tm& time
	)
		: message(message)
		, fileName(fileName)
		, functionName(functionName)
		, line(line)
		, time(time)
	{}

	const std::string message;
	const std::string fileName;
	const std::string functionName;
	const unsigned int line;
	const std::tm time;
};

#endif // LOG_MESSAGE_H