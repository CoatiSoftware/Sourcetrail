#include "utility/file/FileRegister.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"

FileRegister::FileRegister(const FileManager* fileManager)
	: m_fileManager(fileManager)
{
}

void FileRegister::setFilePaths(const std::vector<FilePath>& filePaths)
{
	std::lock_guard<std::mutex> sourceFileLock(m_sourceFileMutex);
	std::lock_guard<std::mutex> includeFileLock(m_includeFileMutex);

	m_sourceFilePaths.clear();
	m_includeFilePaths.clear();

	for (const FilePath& p : filePaths)
	{
		FilePath path = p.exists() ? p.absolute() : p;

		if (m_fileManager->hasSourceExtension(path))
		{
			m_sourceFilePaths.emplace(path, STATE_UNPARSED);
		}
		else if (m_fileManager->hasIncludeExtension(path))
		{
			m_includeFilePaths.emplace(path, STATE_UNPARSED);
		}
	}
}

const FileManager* FileRegister::getFileManager() const
{
	return m_fileManager;
}

std::vector<FilePath> FileRegister::getUnparsedSourceFilePaths() const
{
	std::lock_guard<std::mutex> lock(m_sourceFileMutex);

	std::vector<FilePath> files;

	for (std::pair<FilePath, ParseState>&& p : m_sourceFilePaths)
	{
		if (p.second == STATE_UNPARSED)
		{
			files.push_back(p.first);
		}
	}

	return files;
}

bool FileRegister::hasIncludeFile(const FilePath& filePath) const
{
	std::lock_guard<std::mutex> lock(m_includeFileMutex);

	std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(filePath);
	return (it != m_includeFilePaths.end());
}

bool FileRegister::fileIsParsed(const FilePath& filePath) const
{
	return sourceFileIsParsed(filePath) || includeFileIsParsed(filePath);
}

bool FileRegister::sourceFileIsParsed(const FilePath& filePath) const
{
	{
		std::lock_guard<std::mutex> lock(m_sourceFileMutex);

		std::map<FilePath, ParseState>::const_iterator it = m_sourceFilePaths.find(filePath);
		if (it == m_sourceFilePaths.end())
		{
			return false;
		}

		if (it->second == STATE_UNPARSED)
		{
			return false;
		}
		else if (it->second == STATE_PARSED)
		{
			return true;
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_threadFileMutex);

		std::map<std::thread::id, std::set<FilePath>>::const_iterator it2 = m_threadParsingFiles.find(std::this_thread::get_id());
		if (it2 != m_threadParsingFiles.end())
		{
			if (it2->second.find(filePath) != it2->second.end())
			{
				return false;
			}
		}
		return true;
	}
}

bool FileRegister::includeFileIsParsed(const FilePath& filePath) const
{
	{
		std::lock_guard<std::mutex> lock(m_includeFileMutex);

		std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(filePath);
		if (it == m_includeFilePaths.end())
		{
			return false;
		}

		if (it->second == STATE_UNPARSED)
		{
			return false;
		}
		else if (it->second == STATE_PARSED)
		{
			return true;
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_threadFileMutex);

		std::map<std::thread::id, std::set<FilePath>>::const_iterator it2 = m_threadParsingFiles.find(std::this_thread::get_id());
		if (it2 != m_threadParsingFiles.end())
		{
			if (it2->second.find(filePath) != it2->second.end())
			{
				return false;
			}
		}
		return true;
	}
}

FilePath FileRegister::consumeSourceFile()
{
	std::lock_guard<std::mutex> lock(m_sourceFileMutex);
	for (std::map<FilePath, ParseState>::iterator it = m_sourceFilePaths.begin(); it != m_sourceFilePaths.end(); it++)
	{
		if (it->second == STATE_UNPARSED)
		{
			it->second = STATE_PARSING;
			m_threadParsingFiles[std::this_thread::get_id()].insert(it->first);
			return it->first;
		}
	}
	return FilePath();
}

void FileRegister::markIncludeFileParsing(const FilePath& filePath)
{
	std::lock_guard<std::mutex> lock(m_includeFileMutex);
	std::map<FilePath, ParseState>::iterator it = m_includeFilePaths.find(filePath);
	if (it != m_includeFilePaths.end())
	{
		if (it->second == STATE_UNPARSED)
		{
			it->second = STATE_PARSING;
			m_threadParsingFiles[std::this_thread::get_id()].insert(it->first);
		}
	}
}

void FileRegister::markThreadFilesParsed()
{
	std::lock_guard<std::mutex> sourceFileLock(m_sourceFileMutex);
	std::lock_guard<std::mutex> includeFileLock(m_includeFileMutex);
	std::lock_guard<std::mutex> threadFileLock(m_threadFileMutex);
	for (std::set<FilePath>::iterator it = m_threadParsingFiles[std::this_thread::get_id()].begin(); it != m_threadParsingFiles[std::this_thread::get_id()].end(); it++)
	{
		std::map<FilePath, ParseState>::iterator it2;
		it2 = m_sourceFilePaths.find(*it);
		if (it2 != m_sourceFilePaths.end())
		{
			it2->second = STATE_PARSED;
			continue;
		}

		it2 = m_includeFilePaths.find(*it);
		if (it2 != m_includeFilePaths.end())
		{
			it2->second = STATE_PARSED;
		}
	}
	m_threadParsingFiles[std::this_thread::get_id()].clear();
}

size_t FileRegister::getSourceFilesCount() const
{
	std::lock_guard<std::mutex> lock(m_sourceFileMutex);
	return m_sourceFilePaths.size();
}

size_t FileRegister::getParsedSourceFilesCount() const
{
	return getSourceFilesCount() - getUnparsedSourceFilePaths().size();
}
