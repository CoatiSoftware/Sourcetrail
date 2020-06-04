#include "FileRegister.h"

#include "FilePath.h"
#include "FilePathFilter.h"

FileRegister::FileRegister(
	const FilePath& currentPath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePathFilter>& excludeFilters)
	: m_currentPath(currentPath)
	, m_indexedPaths(indexedPaths)
	, m_excludeFilters(excludeFilters)
	, m_hasFilePathCache([&](const std::wstring& f) {
		const FilePath filePath(f);
		bool ret = false;

		if (filePath == m_currentPath)
		{
			ret = true;
		}

		if (!ret)
		{
			for (const FilePath& indexedPath: m_indexedPaths)
			{
				if (indexedPath.isDirectory())
				{
					if (indexedPath.contains(filePath))
					{
						ret = true;
						break;
					}
				}
				else
				{
					if (indexedPath == filePath)
					{
						ret = true;
						break;
					}
				}
			}
		}

		if (ret)
		{
			ret = !FilePathFilter::areMatching(m_excludeFilters, filePath);
		}
		return ret;
	})
{
}

FileRegister::~FileRegister() {}

bool FileRegister::hasFilePath(const FilePath& filePath) const
{
	return m_hasFilePathCache.getValue(filePath.wstr());
}
