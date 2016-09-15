#include "data/StorageProvider.h"

#include <iostream>

int StorageProvider::getStorageCount() const
{
	std::lock_guard<std::mutex> lock(m_storagesMutex);
	return m_storages.size();
}

void StorageProvider::pushIndexerTarget(std::shared_ptr<IntermediateStorage> storage)
{
	std::lock_guard<std::mutex> lock(m_storagesMutex);
	m_storages.push_back(storage);
}

std::shared_ptr<IntermediateStorage> StorageProvider::popIndexerTarget()
{
	return std::make_shared<IntermediateStorage>();;
}

std::shared_ptr<IntermediateStorage> StorageProvider::popInjectionSource()
{
	std::shared_ptr<IntermediateStorage> ret;
	{
		std::lock_guard<std::mutex> lock(m_storagesMutex);
		if (!m_storages.empty())
		{
			ret = m_storages.front();
			m_storages.pop_front();
		}
	}

	return ret;
}
