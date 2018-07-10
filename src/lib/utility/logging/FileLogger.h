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

	FilePath getLogFilePath() const;
	void setLogFilePath(const FilePath& filePath);

	void setLogDirectory(const FilePath& filePath);
	void setFileName(const std::wstring& fileName);
	void setMaxLogLineCount(unsigned int logCount);

	// setting the max log file count to 0 will disable ringlogging
	void setMaxLogFileCount(unsigned int amount);

private:
	void logInfo(const LogMessage& message) override;
	void logWarning(const LogMessage& message) override;
	void logError(const LogMessage& message) override;

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
