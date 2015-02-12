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

private:
	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	static const std::string s_filePath;

	void setupFileName();
	void logMessage(const std::string& type, const LogMessage& message);

	std::string m_fileName;
};

#endif // FILE_LOGGER_H
