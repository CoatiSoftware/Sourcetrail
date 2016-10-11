#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include <string>

#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"

class FileLogger: public Logger
{
public:
	FileLogger();
	virtual ~FileLogger();

	void setLogDirectory(const std::string& filePath);
	void setMaxLogLineCount(unsigned int logCount);

	// setting the max log file count to 0 will disable ringlogging
	void setMaxLogFileCount(unsigned int amount);

private:
	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	static std::string getFileName();
	void logMessage(const std::string& type, const LogMessage& message);
	void updateLogFileName();

	const std::string m_logFileName;
	std::string m_logDirectory;
	unsigned int m_maxLogLineCount;
	unsigned int m_maxLogFileCount;
	unsigned int m_currentLogLineCount;
	unsigned int m_currentLogFileCount;

	std::string m_currentLogFileName;
};

#endif // FILE_LOGGER_H
