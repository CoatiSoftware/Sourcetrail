#include "FileLogger.h"

#include <fstream>
#include <sstream>
#include <cstdio>

#include "utility/file/FileSystem.h"

FileLogger::FileLogger()
	: Logger("FileLogger")
	, m_logFileName("log")
	, m_logDirectory(L"user/log/")
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

FilePath FileLogger::getLogFilePath() const
{
	return m_currentLogFilePath;
}

void FileLogger::setLogFilePath(const FilePath& filePath)
{
	m_currentLogFilePath = filePath;
	m_logFileName = "";
}

void FileLogger::setLogDirectory(const FilePath& filePath)
{
	m_logDirectory = filePath;
	FileSystem::createDirectory(m_logDirectory);
}

void FileLogger::setFileName(const std::string& fileName)
{
	if (fileName != m_logFileName)
	{
		m_logFileName = fileName;
		m_currentLogLineCount = 0;
		m_currentLogFileCount = 0;
		updateLogFileName();
	}
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
	if (!m_logFileName.size())
	{
		return;
	}

	bool fileChanged = false;

	std::string currentLogFilePath = m_logDirectory.str() + m_logFileName;
	if (m_maxLogFileCount > 0)
	{
		currentLogFilePath += "_";
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
		currentLogFilePath += std::to_string(m_currentLogFileCount);

	}
	currentLogFilePath += ".txt";

	m_currentLogFilePath = FilePath(currentLogFilePath);

	if (fileChanged)
	{
		FileSystem::remove(m_currentLogFilePath);
	}
}

void FileLogger::logMessage(const std::string& type, const LogMessage& message)
{
	std::ofstream fileStream;
	fileStream.open(m_currentLogFilePath.str(), std::ios::app);
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
