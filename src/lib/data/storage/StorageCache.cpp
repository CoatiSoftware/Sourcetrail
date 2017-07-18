#include "data/storage/StorageCache.h"

void StorageCache::clear()
{
	m_graphForAll.reset();

	m_storageStats = StorageStats();
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
