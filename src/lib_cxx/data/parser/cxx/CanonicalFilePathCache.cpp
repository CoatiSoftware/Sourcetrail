#include "data/parser/cxx/CanonicalFilePathCache.h"

#include "utility/utilityString.h"
#include "data/parser/cxx/utilityClang.h"

CanonicalFilePathCache::CanonicalFilePathCache(std::shared_ptr<FileRegister> fileRegister)
	: m_fileRegister(fileRegister)
{
}

std::shared_ptr<FileRegister> CanonicalFilePathCache::getFileRegister() const
{
	return m_fileRegister;
}

FilePath CanonicalFilePathCache::getCanonicalFilePath(const clang::FileID& fileId, const clang::SourceManager& sourceManager)
{
	if (!fileId.isValid())
	{
		return FilePath();
	}

	auto it = m_fileIdMap.find(fileId);
	if (it != m_fileIdMap.end())
	{
		return it->second;
	}

	FilePath filePath;

	const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		filePath = getCanonicalFilePath(fileEntry);
		m_fileIdMap.emplace(fileId, filePath);
	}

	return filePath;
}

FilePath CanonicalFilePathCache::getCanonicalFilePath(const clang::FileEntry* entry)
{
	return getCanonicalFilePath(utility::getFileNameOfFileEntry(entry));
}

FilePath CanonicalFilePathCache::getCanonicalFilePath(const std::wstring& path)
{
	const std::wstring lowercasePath = utility::toLowerCase(path);

	auto it = m_fileStringMap.find(lowercasePath);
	if (it != m_fileStringMap.end())
	{
		return it->second;
	}

	const FilePath canonicalPath = FilePath(path).makeCanonical();
	const std::wstring lowercaseCanonicalPath = utility::toLowerCase(canonicalPath.wstr());

	m_fileStringMap.emplace(std::move(lowercasePath), canonicalPath);
	m_fileStringMap.emplace(std::move(lowercaseCanonicalPath), canonicalPath);

	return canonicalPath;
}

bool CanonicalFilePathCache::isProjectFile(const clang::FileID fileId, const clang::SourceManager& sourceManager)
{
	if (!fileId.isValid())
	{
		return false;
	}

	auto it = m_isProjectFileMap.find(fileId);
	if (it != m_isProjectFileMap.end())
	{
		return it->second;
	}

	bool ret = m_fileRegister->hasFilePath(getCanonicalFilePath(fileId, sourceManager));
	m_isProjectFileMap.emplace(fileId, ret);
	return ret;
}
