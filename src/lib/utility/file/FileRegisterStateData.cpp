#include "utility/file/FileRegisterStateData.h"

FileRegisterStateData::FileRegisterStateData()
{
}

FileRegisterStateData::FileRegisterStateData(const FileRegisterStateData& o)
{
	this->inject(o);
}

void FileRegisterStateData::inject(const FileRegisterStateData& o)
{
	std::lock_guard<std::mutex> oLock(o.m_filePathsMutex);
	std::lock_guard<std::mutex> thisLock(m_filePathsMutex);
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
	std::lock_guard<std::mutex> lock(m_filePathsMutex);
	m_filePaths[filePath] = STATE_INDEXING;
}

void FileRegisterStateData::markIndexingFilesIndexed()
{
	std::lock_guard<std::mutex> lock(m_filePathsMutex);
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
	std::lock_guard<std::mutex> lock(m_filePathsMutex);
	auto it = m_filePaths.find(filePath);
	if (it != m_filePaths.end())
	{
		return it->second == STATE_INDEXED;
	}

	return false;
}
