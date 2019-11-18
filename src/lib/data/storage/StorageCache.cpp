#include "StorageCache.h"

#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "TextAccess.h"
#include "utility.h"

void StorageCache::clear()
{
	m_graphForAll.reset();

	m_storageStats = StorageStats();

	setUseErrorCache(false);
}

std::shared_ptr<Graph> StorageCache::getGraphForAll() const
{
	if (!m_graphForAll)
	{
		m_graphForAll = StorageAccessProxy::getGraphForAll();
	}

	return m_graphForAll;
}

StorageStats StorageCache::getStorageStats() const
{
	if (!m_storageStats.nodeCount)
	{
		m_storageStats = StorageAccessProxy::getStorageStats();
	}

	return m_storageStats;
}

std::shared_ptr<TextAccess> StorageCache::getFileContent(const FilePath& filePath, bool showsErrors) const
{
	if (m_useErrorCache && showsErrors)
	{
		return TextAccess::createFromFile(filePath);
	}

	return StorageAccessProxy::getFileContent(filePath, showsErrors);
}

ErrorCountInfo StorageCache::getErrorCount() const
{
	if (!m_useErrorCache)
	{
		return StorageAccessProxy::getErrorCount();
	}

	return m_errorCount;
}

std::vector<ErrorInfo> StorageCache::getErrorsLimited(const ErrorFilter& filter) const
{
	if (!m_useErrorCache)
	{
		return StorageAccessProxy::getErrorsLimited(filter);
	}

	return filter.filterErrors(m_cachedErrors);
}

std::vector<ErrorInfo> StorageCache::getErrorsForFileLimited(
	const ErrorFilter& filter, const FilePath& filePath) const
{
	if (!m_useErrorCache)
	{
		return StorageAccessProxy::getErrorsForFileLimited(filter, filePath);
	}

	return {};
}

std::shared_ptr<SourceLocationCollection> StorageCache::getErrorSourceLocations(
	const std::vector<ErrorInfo>& errors) const
{
	std::shared_ptr<SourceLocationCollection> collection =
		StorageAccessProxy::getErrorSourceLocations(errors);

	if (m_useErrorCache)
	{
		std::map<std::wstring, bool> fileIndexed;
		for (const ErrorInfo& error: m_cachedErrors)
		{
			fileIndexed.emplace(error.filePath, error.indexed);
		}

		collection->forEachSourceLocationFile([&](std::shared_ptr<SourceLocationFile> file) {
			file->setIsComplete(false);

			auto it = fileIndexed.find(file->getFilePath().wstr());
			if (it != fileIndexed.end())
			{
				file->setIsIndexed(it->second);
			}
			else
			{
				file->setIsIndexed(true);
			}
		});
	}

	return collection;
}


void StorageCache::setUseErrorCache(bool enabled)
{
	m_useErrorCache = enabled;
	m_cachedErrors.clear();
	m_errorCount = ErrorCountInfo();
}

void StorageCache::addErrorsToCache(
	const std::vector<ErrorInfo>& newErrors, const ErrorCountInfo& errorCount)
{
	utility::append(m_cachedErrors, newErrors);
	m_errorCount = errorCount;
}
