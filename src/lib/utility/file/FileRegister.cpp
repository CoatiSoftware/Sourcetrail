#include "utility/file/FileRegister.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"

FileRegister::FileRegister(const FileManager* fileManager)
	: m_fileManager(fileManager)
{
}

const FileManager* FileRegister::getFileManager() const
{
	return m_fileManager;
}

void FileRegister::setFilePaths(const std::vector<FilePath>& filePaths)
{
	m_sourceFilePaths.clear();
	m_includeFilePaths.clear();

	for (const FilePath& p : filePaths)
	{
		FilePath path = p.exists() ? p.absolute() : p;

		if (m_fileManager->hasSourceExtension(path))
		{
			m_sourceFilePaths.emplace(path, STATE_UNPARSED);
		}
		else
		{
			m_includeFilePaths.emplace(path, STATE_UNPARSED);
		}
	}
}

std::vector<FilePath> FileRegister::getUnparsedSourceFilePaths() const
{
	return getUnparsedFilePaths(m_sourceFilePaths);
}

std::vector<FilePath> FileRegister::getUnparsedIncludeFilePaths() const
{
	return getUnparsedFilePaths(m_includeFilePaths);
}

bool FileRegister::fileIsParsed(const FilePath& filePath) const
{
	std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(filePath);
	if (it != m_includeFilePaths.end())
	{
		return it->second == STATE_PARSED;
	}

	it = m_sourceFilePaths.find(filePath);
	if (it != m_sourceFilePaths.end())
	{
		return it->second == STATE_PARSED;
	}

	return true;
}

bool FileRegister::includeFileIsParsing(const FilePath& filePath) const
{
	std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(filePath);
	if (it == m_includeFilePaths.end())
	{
		return false;
	}

	return it->second == STATE_PARSING;
}

bool FileRegister::includeFileIsParsed(const FilePath& filePath) const
{
	std::map<FilePath, ParseState>::const_iterator it = m_includeFilePaths.find(filePath);
	if (it == m_includeFilePaths.end())
	{
		return false;
	}

	return it->second == STATE_PARSED;
}

void FileRegister::markSourceFileParsed(const std::string& filePath)
{
	std::map<FilePath, ParseState>::iterator it = m_sourceFilePaths.find(FilePath(filePath));
	if (it == m_sourceFilePaths.end())
	{
		return;
	}

	it->second = STATE_PARSED;
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
	for (auto& p : m_includeFilePaths)
	{
		if (p.second == STATE_PARSING)
		{
			p.second = STATE_PARSED;
		}
	}
}

std::vector<FilePath> FileRegister::getUnparsedFilePaths(const std::map<FilePath, ParseState> filePaths) const
{
	std::vector<FilePath> files;

	for (std::pair<FilePath, ParseState>&& p : filePaths)
	{
		if (p.second == STATE_UNPARSED)
		{
			files.push_back(p.first);
		}
	}

	return files;
}

size_t FileRegister::getFilesCount() const
{
	return m_sourceFilePaths.size() + m_includeFilePaths.size();
}

size_t FileRegister::getParsedFilesCount() const
{
	return getFilesCount() - getUnparsedSourceFilePaths().size() - getUnparsedIncludeFilePaths().size();
}
