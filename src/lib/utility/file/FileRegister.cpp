#include "utility/file/FileRegister.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"

FileRegister::FileRegister(const FileManager* fileManager, bool randomizeParseOrder)
	: m_fileManager(fileManager)
	, m_randomizeParseOrder(randomizeParseOrder)
{
}

void FileRegister::setFilePaths(const std::vector<FilePath>& filePaths)
{
	{
		std::lock_guard<std::mutex> lock(m_sourceFileMutex);
		m_sourceFilePaths.clear();

		for (const FilePath& p : filePaths)
		{
			FilePath path = p.exists() ? p.absolute() : p;

			if (m_fileManager->hasSourceExtension(path))
			{
				m_sourceFilePaths.emplace(path, STATE_UNPARSED);
			}
		}
	}

	{
		std::lock_guard<std::mutex> lock(m_includeFileMutex);
		m_includeFilePaths.clear();
	}

	{
		std::lock_guard<std::mutex> lock(m_threadFileMutex);
		m_threadParsingFiles.clear();
	}
}

bool FileRegister::hasFilePath(const FilePath& filePath) const
{
	std::lock_guard<std::mutex> lock(m_projectFilesMutex);
	std::unordered_map<std::string, bool>::iterator it = m_projectFiles.find(filePath.str());
	if (it != m_projectFiles.end())
	{
		return it->second;
	}

	bool has = m_fileManager->hasFilePath(filePath);
	m_projectFiles.emplace(filePath.str(), has);

	return has;
}

const FileInfo FileRegister::getFileInfo(const FilePath& filePath) const
{
	std::lock_guard<std::mutex> lock(m_projectFileInfosMutex);
	std::unordered_map<std::string, FileInfo>::iterator it = m_projectFileInfos.find(filePath.str());
	if (it != m_projectFileInfos.end())
	{
		return it->second;
	}

	FileInfo info = m_fileManager->getFileInfo(filePath);
	m_projectFileInfos.emplace(filePath.str(), info);

	return info;
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

		if (it->second == STATE_PARSED)
		{
			return true;
		}

		return false;
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

		if (it->second == STATE_PARSED)
		{
			return true;
		}

		return false;
	}
}

FilePath FileRegister::consumeSourceFile()
{
	std::vector<FilePath> paths = getUnparsedSourceFilePaths();

	FilePath path;

	if (paths.size())
	{
		if (m_randomizeParseOrder)
		{
			path = paths[rand() % paths.size()];
		}
		else
		{
			path = paths[0];
		}

		std::lock_guard<std::mutex> lock(m_sourceFileMutex);
		m_sourceFilePaths[path] = STATE_PARSING;
	}

	if (!path.empty())
	{
		std::lock_guard<std::mutex> lock(m_threadFileMutex);
		m_threadParsingFiles[std::this_thread::get_id()].insert(path);
	}

	return path;
}

void FileRegister::markIncludeFileParsing(const FilePath& filePath)
{
	bool unparsed = false;
	{
		std::lock_guard<std::mutex> lock(m_includeFileMutex);
		std::map<FilePath, ParseState>::iterator it = m_includeFilePaths.find(filePath);

		if (it != m_includeFilePaths.end())
		{
			if (it->second == STATE_UNPARSED)
			{
				it->second = STATE_PARSING;
				unparsed = true;
			}
		}
		else
		{
			m_includeFilePaths.emplace(filePath, STATE_PARSING);
			unparsed = true;
		}
	}

	if (unparsed)
	{
		std::lock_guard<std::mutex> lock(m_threadFileMutex);
		m_threadParsingFiles[std::this_thread::get_id()].insert(filePath);
	}
}

void FileRegister::markThreadFilesParsed()
{
	std::lock_guard<std::mutex> sourceFileLock(m_sourceFileMutex);
	std::lock_guard<std::mutex> includeFileLock(m_includeFileMutex);
	std::lock_guard<std::mutex> threadFileLock(m_threadFileMutex);

	std::set<FilePath>& threadFiles = m_threadParsingFiles[std::this_thread::get_id()];

	for (std::set<FilePath>::iterator it = threadFiles.begin(); it != threadFiles.end(); it++)
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

	threadFiles.clear();
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
