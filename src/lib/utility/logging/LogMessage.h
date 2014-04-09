#ifndef LOG_MESSAGE_H
#define LOG_MESSAGE_H

#include <ctime>
#include <string>

struct LogMessage
{
public:
	LogMessage(
		const std::string& message,
		const std::string& filePath,
		const std::string& functionName,
		const unsigned int line,
		const std::tm& time
	)
		: message(message)
		, filePath(filePath)
		, functionName(functionName)
		, line(line)
		, time(time)
	{}

	std::string getTimeString(const std::string& format) const
	{
		char timeString[50];
		strftime(timeString, 50, format.c_str(), &time);
		return std::string(timeString);
	}

	std::string getFileName() const
	{
		return filePath.substr(filePath.find_last_of("/\\") + 1);
	}

	const std::string message;
	const std::string filePath;
	const std::string functionName;
	const unsigned int line;
	const std::tm time;
};

#endif // LOG_MESSAGE_H
