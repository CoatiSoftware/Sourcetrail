#include "utility/file/FileRegister.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"

FileRegister::FileRegister(const FileManager* fileManager, const std::vector<FilePath>& filePaths)
	: m_fileManager(fileManager)
{
	for (const FilePath& path : filePaths)
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

const std::vector<FilePath>& FileRegister::getSourceFilePaths() const
{
	return m_sourceFilePaths;
}

bool FileRegister::includeFileIsParsing(const std::string& filePath) const
{
	std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(FilePath(filePath));
	if (it == m_includeFilePaths.end())
	{
		return false;
	}

	return it->second == STATE_PARSING;
}

void FileRegister::markIncludeFileParsing(const std::string& filePath)
{
	std::map<FilePath, ParseState>::iterator it = m_includeFilePaths.find(FilePath(filePath));
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
	for (std::pair<FilePath, ParseState>&& p : m_includeFilePaths)
	{
		if (p.second == STATE_PARSING)
		{
			p.second = STATE_PARSED;
		}
	}
}
