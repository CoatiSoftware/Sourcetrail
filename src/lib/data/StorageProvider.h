#ifndef STORAGE_PROVIDER_H
#define STORAGE_PROVIDER_H

#include <memory>
#include <mutex>
#include <list>
#include "data/IntermediateStorage.h"

class StorageProvider
{
public:
	int getStorageCount() const;
	void pushIndexerTarget(std::shared_ptr<IntermediateStorage> storage);

	// always returns a usable storage
	std::shared_ptr<IntermediateStorage> popIndexerTarget();

	// returns empty shared_ptr if no storages available
	std::shared_ptr<IntermediateStorage> popInjectionSource();

private:
	std::list<std::shared_ptr<IntermediateStorage>> m_storages; // larger storages are in front
	mutable std::mutex m_storagesMutex;
};

#endif // STORAGE_PROVIDER_H
