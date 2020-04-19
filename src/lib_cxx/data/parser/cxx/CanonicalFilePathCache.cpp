#include "CanonicalFilePathCache.h"

#include <clang/AST/ASTContext.h>

#include "utilityClang.h"
#include "utilityString.h"

CanonicalFilePathCache::CanonicalFilePathCache(std::shared_ptr<FileRegister> fileRegister)
	: m_fileRegister(fileRegister)
{
}

std::shared_ptr<FileRegister> CanonicalFilePathCache::getFileRegister() const
{
	return m_fileRegister;
}

FilePath CanonicalFilePathCache::getCanonicalFilePath(
	const clang::FileID& fileId, const clang::SourceManager& sourceManager)
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

FilePath CanonicalFilePathCache::getCanonicalFilePath(const Id symbolId)
{
	auto it = m_symbolIdFileIdMap.find(symbolId);
	if (it != m_symbolIdFileIdMap.end())
	{
		auto it2 = m_fileIdMap.find(it->second);
		if (it2 != m_fileIdMap.end())
		{
			return it2->second;
		}
	}

	return FilePath();
}

void CanonicalFilePathCache::addFileSymbolId(const clang::FileID& fileId, const FilePath& path, Id symbolId)
{
	m_fileIdSymbolIdMap.emplace(fileId, symbolId);
	m_symbolIdFileIdMap.emplace(symbolId, fileId);
	m_fileStringSymbolIdMap.emplace(utility::toLowerCase(path.wstr()), symbolId);
}

Id CanonicalFilePathCache::getFileSymbolId(const clang::FileID& fileId)
{
	if (!fileId.isValid())
	{
		return 0;
	}

	auto it = m_fileIdSymbolIdMap.find(fileId);
	if (it != m_fileIdSymbolIdMap.end())
	{
		return it->second;
	}

	return 0;
}

Id CanonicalFilePathCache::getFileSymbolId(const clang::FileEntry* entry)
{
	return getFileSymbolId(utility::getFileNameOfFileEntry(entry));
}

Id CanonicalFilePathCache::getFileSymbolId(const std::wstring& path)
{
	std::wstring canonicalPath = utility::toLowerCase(getCanonicalFilePath(path).wstr());

	auto it = m_fileStringSymbolIdMap.find(canonicalPath);
	if (it != m_fileStringSymbolIdMap.end())
	{
		return it->second;
	}

	return 0;
}

FilePath CanonicalFilePathCache::getDeclarationFilePath(const clang::Decl* declaration)
{
	const clang::SourceManager& sourceManager = declaration->getASTContext().getSourceManager();
	const clang::FileID fileId = sourceManager.getFileID(declaration->getBeginLoc());
	const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		return getCanonicalFilePath(fileId, sourceManager);
	}
	return getCanonicalFilePath(utility::decodeFromUtf8(
		sourceManager.getPresumedLoc(declaration->getBeginLoc()).getFilename()));
}

std::wstring CanonicalFilePathCache::getDeclarationFileName(const clang::Decl* declaration)
{
	return getDeclarationFilePath(declaration).fileName();
}

bool CanonicalFilePathCache::isProjectFile(
	const clang::FileID& fileId, const clang::SourceManager& sourceManager)
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
