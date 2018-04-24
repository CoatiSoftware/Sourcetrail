#include "utility/file/FileRegister.h"

#include "utility/file/FilePath.h"
#include "utility/file/FilePathFilter.h"

FileRegister::FileRegister(
	const FilePath& currentPath,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePathFilter>& excludeFilters
)
	: m_currentPath(currentPath)
	, m_indexedPaths(indexedPaths)
	, m_excludeFilters(excludeFilters)
	, m_hasFilePathCache(
		[&](const std::wstring& f)
		{
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
				for (const FilePathFilter& excludeFilter: m_excludeFilters)
				{
					if (excludeFilter.isMatching(filePath))
					{
						ret = false;
						break;
					}
				}
			}
			return ret;
		}
	)
{
}

FileRegister::~FileRegister()
{
}

bool FileRegister::hasFilePath(const FilePath& filePath) const
{
	return m_hasFilePathCache.getValue(filePath.wstr());
}
