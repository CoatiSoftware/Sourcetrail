#include "utility/file/FileRegisterStateData.h"

#include "utility/file/FilePath.h"

FileRegisterStateData::FileRegisterStateData()
{
}

FileRegisterStateData::FileRegisterStateData(const FileRegisterStateData& o)
{
	this->inject(o);
}

void FileRegisterStateData::inject(const FileRegisterStateData& o)
{
	for (const auto& it: o.m_filePaths)
	{
		if (it.second == STATE_INDEXED)
		{
			m_filePaths[it.first] = STATE_INDEXED;
		}
	}
}

void FileRegisterStateData::markFileIndexing(const FilePath& filePath)
{
	m_filePaths[filePath] = STATE_INDEXING;
}

void FileRegisterStateData::markIndexingFilesIndexed()
{
	for (auto& it: m_filePaths)
	{
		if (it.second == STATE_INDEXING)
		{
			it.second = STATE_INDEXED;
		}
	}
}

bool FileRegisterStateData::fileIsIndexed(const FilePath& filePath) const
{
	auto it = m_filePaths.find(filePath);
	if (it != m_filePaths.end())
	{
		return it->second == STATE_INDEXED;
	}

	return false;
}

void FileRegisterStateData::setIndexedFiles(const std::set<FilePath>& filePaths)
{
	for (auto path : filePaths)
	{
		m_filePaths[path] = STATE_INDEXED;
	}
}

std::set<FilePath> FileRegisterStateData::getIndexedFiles() const
{
	std::set<FilePath> paths;
	for (auto& it : m_filePaths)
	{
		if (it.second == STATE_INDEXED)
		{
			paths.insert(it.first);
		}
	}
	return paths;
}
