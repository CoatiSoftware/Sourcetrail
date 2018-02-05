#include "data/parser/cxx/CanonicalFilePathCache.h"

#include "utility/utilityString.h"
#include "data/parser/cxx/utilityClang.h"


FilePath CanonicalFilePathCache::getCanonicalFilePath(const clang::FileEntry* entry)
{
	return getCanonicalFilePath(utility::getFileNameOfFileEntry(entry));
}

FilePath CanonicalFilePathCache::getCanonicalFilePath(const std::wstring& path)
{
	const std::wstring lowercasePath = utility::toLowerCase(path);

	std::unordered_map<std::wstring, FilePath>::const_iterator it = m_map.find(lowercasePath);
	if (it != m_map.end())
	{
		return it->second;
	}

	const FilePath canonicalPath = FilePath(path).makeCanonical();
	const std::wstring lowercaseCanonicalPath = utility::toLowerCase(canonicalPath.wstr());

	m_map.insert(std::make_pair(lowercasePath, canonicalPath));
	m_map.insert(std::make_pair(lowercaseCanonicalPath, canonicalPath));

	return canonicalPath;
}
