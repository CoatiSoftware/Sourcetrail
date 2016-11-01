#include "FileLogger.h"

#include <fstream>
#include <sstream>
#include <cstdio>

#include "utility/file/FileSystem.h"

FileLogger::FileLogger()
	: Logger("FileLogger")
	, m_logFileName(getFileName())
	, m_logDirectory("user/log/")
	, m_maxLogLineCount(0)
	, m_maxLogFileCount(0)
	, m_currentLogLineCount(0)
	, m_currentLogFileCount(0)
{
	updateLogFileName();
}

FileLogger::~FileLogger()
{
}

void FileLogger::setLogDirectory(const std::string& filePath)
{
	m_logDirectory = filePath;
	FileSystem::createDirectory(m_logDirectory);
}

void FileLogger::logInfo(const LogMessage& message)
{
	logMessage("INFO", message);
}

void FileLogger::logWarning(const LogMessage& message)
{
	logMessage("WARNING", message);
}

void FileLogger::logError(const LogMessage& message)
{
	logMessage("ERROR", message);
}

std::string FileLogger::getFileName()
{
	time_t time;
	std::time(&time);
	tm t = *std::localtime(&time);

	std::stringstream filename;
	filename << "log_";
	filename << t.tm_year + 1900 << "-";
	filename << (t.tm_mon < 9 ? "0" : "") << t.tm_mon + 1 << "-";
	filename << (t.tm_mday < 10 ? "0" : "") << t.tm_mday << "_";
	filename << (t.tm_hour < 10 ? "0" : "") << t.tm_hour << "-";
	filename << (t.tm_min < 10 ? "0" : "") << t.tm_min << "-";
	filename << (t.tm_sec < 10 ? "0" : "") << t.tm_sec;

	return filename.str();
}

void FileLogger::setMaxLogLineCount(unsigned int lineCount)
{
	m_maxLogLineCount = lineCount;
}

void FileLogger::setMaxLogFileCount(unsigned int fileCount)
{
	m_maxLogFileCount = fileCount;
}

void FileLogger::updateLogFileName()
{
	bool fileChanged = false;

	m_currentLogFileName = m_logFileName;
	if (m_maxLogFileCount > 0)
	{
		m_currentLogFileName += "_";
		if (m_currentLogLineCount >= m_maxLogLineCount)
		{
			m_currentLogLineCount = 0;

			m_currentLogFileCount++;
			if (m_currentLogFileCount >= m_maxLogFileCount)
			{
				m_currentLogFileCount = 0;
			}
			fileChanged = true;
		}
		m_currentLogFileName += std::to_string(m_currentLogFileCount);

	}
	m_currentLogFileName += ".txt";

	if (fileChanged)
	{
		FileSystem::remove(m_logDirectory + m_currentLogFileName);
	}
}

void FileLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::ofstream fileStream;
	fileStream.open(m_logDirectory + m_currentLogFileName, std::ios::app);
	fileStream << message.getTimeString("%H:%M:%S") << " | ";
	fileStream << message.threadId << " | ";

	if (message.filePath.size())
	{
		fileStream << message.getFileName() << ':' << message.line << ' ' << message.functionName << "() | ";
	}

	fileStream << type << ": " << message.message << std::endl;
	fileStream.close();

	m_currentLogLineCount++;
	if (m_maxLogFileCount > 0)
	{
		updateLogFileName();
	}
}
