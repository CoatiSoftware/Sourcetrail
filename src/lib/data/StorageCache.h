#ifndef STORAGE_CACHE_H
#define STORAGE_CACHE_H

#include <map>

#include "data/access/StorageAccessProxy.h"

class StorageCache
	: public StorageAccessProxy
{
public:
	void clear();

	virtual std::shared_ptr<Graph> getGraphForAll() const;

	virtual StorageStats getStorageStats() const;

private:
	mutable std::shared_ptr<Graph> m_graphForAll;
	mutable StorageStats m_storageStats;
};

#endif // STORAGE_CACHE_H
