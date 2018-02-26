#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include <string>

#include "utility/file/FilePath.h"
#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"

class FileLogger: public Logger
{
public:
	static std::wstring generateDatedFileName(const std::wstring& prefix = L"", const std::wstring& suffix = L"");

	FileLogger();
	virtual ~FileLogger();

	FilePath getLogFilePath() const;
	void setLogFilePath(const FilePath& filePath);

	void setLogDirectory(const FilePath& filePath);
	void setFileName(const std::wstring& fileName);
	void setMaxLogLineCount(unsigned int logCount);

	// setting the max log file count to 0 will disable ringlogging
	void setMaxLogFileCount(unsigned int amount);

private:
	virtual void logInfo(const LogMessage& message);
	virtual void logWarning(const LogMessage& message);
	virtual void logError(const LogMessage& message);

	void logMessage(const std::string& type, const LogMessage& message);
	void updateLogFileName();

	std::wstring m_logFileName;
	FilePath m_logDirectory;
	FilePath m_currentLogFilePath;

	unsigned int m_maxLogLineCount;
	unsigned int m_maxLogFileCount;
	unsigned int m_currentLogLineCount;
	unsigned int m_currentLogFileCount;
};

#endif // FILE_LOGGER_H
