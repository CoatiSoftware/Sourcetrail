#ifndef STORAGE_PROVIDER_H
#define STORAGE_PROVIDER_H

#include <memory>
#include <mutex>
#include <list>
#include "IntermediateStorage.h"

class StorageProvider
{
public:
	int getStorageCount() const;

	void insert(std::shared_ptr<IntermediateStorage> storage);

	// returns empty shared_ptr if no storages available
	std::shared_ptr<IntermediateStorage> consumeSecondLargestStorage();

	// returns empty shared_ptr if no storages available
	std::shared_ptr<IntermediateStorage> consumeLargestStorage();

	void logCurrentState() const;

private:
	std::list<std::shared_ptr<IntermediateStorage>> m_storages; // larger storages are in front
	mutable std::mutex m_storagesMutex;
};

#endif // STORAGE_PROVIDER_H
