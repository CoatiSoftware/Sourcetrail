#include "utility/file/FileRegister.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"

FileRegister::FileRegister(const FileManager* fileManager, const std::vector<std::string>& filePaths)
	: m_fileManager(fileManager)
{
	for (const std::string& path : filePaths)
	{
		if (m_fileManager->hasSourceExtension(path))
		{
			m_sourceFilePaths.push_back(path);
		}
		else
		{
			m_includeFilePaths.emplace(path, STATE_UNPARSED);
		}
	}
}

const FileManager* FileRegister::getFileManager() const
{
	return m_fileManager;
}

const std::vector<std::string>& FileRegister::getSourceFilePaths() const
{
	return m_sourceFilePaths;
}

bool FileRegister::includeFileIsParsing(const std::string& filePath) const
{
	std::map<std::string, ParseState>::const_iterator it = m_includeFilePaths.find(FileSystem::absoluteFilePath(filePath));
	if (it == m_includeFilePaths.end())
	{
		return false;
	}

	return it->second == STATE_PARSING;
}

void FileRegister::markIncludeFileParsing(const std::string& filePath)
{
	std::map<std::string, ParseState>::iterator it = m_includeFilePaths.find(FileSystem::absoluteFilePath(filePath));
	if (it == m_includeFilePaths.end())
	{
		return;
	}

	if (it->second != STATE_PARSED)
	{
		it->second = STATE_PARSING;
	}
}

void FileRegister::markParsingIncludeFilesParsed()
{
	for (std::pair<std::string, ParseState>&& p : m_includeFilePaths)
	{
		if (p.second == STATE_PARSING)
		{
			p.second = STATE_PARSED;
		}
	}
}
