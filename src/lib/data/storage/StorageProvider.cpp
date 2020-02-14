#include "StorageProvider.h"

#include "logging.h"

int StorageProvider::getStorageCount() const
{
	std::lock_guard<std::mutex> lock(m_storagesMutex);
	return static_cast<int>(m_storages.size());
}

void StorageProvider::clear()
{
	std::lock_guard<std::mutex> lock(m_storagesMutex);
	return m_storages.clear();
}

void StorageProvider::insert(std::shared_ptr<IntermediateStorage> storage)
{
	const std::size_t storageSize = storage->getSourceLocationCount();
	std::list<std::shared_ptr<IntermediateStorage>>::iterator it;

	std::lock_guard<std::mutex> lock(m_storagesMutex);
	for (it = m_storages.begin(); it != m_storages.end(); it++)
	{
		if ((*it)->getSourceLocationCount() < storageSize)
		{
			break;
		}
	}
	m_storages.insert(it, storage);
}

std::shared_ptr<IntermediateStorage> StorageProvider::consumeSecondLargestStorage()
{
	std::shared_ptr<IntermediateStorage> ret;
	{
		std::lock_guard<std::mutex> lock(m_storagesMutex);
		if (m_storages.size() > 1)
		{
			std::list<std::shared_ptr<IntermediateStorage>>::iterator it = m_storages.begin();
			it++;
			ret = *it;
			m_storages.erase(it);
		}
	}
	return ret;
}

std::shared_ptr<IntermediateStorage> StorageProvider::consumeLargestStorage()
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

void StorageProvider::logCurrentState() const
{
	std::string logString = "Storages waiting for injection:";
	{
		std::lock_guard<std::mutex> lock(m_storagesMutex);
		for (const std::shared_ptr<IntermediateStorage>& storage: m_storages)
		{
			logString += " " + std::to_string(storage->getSourceLocationCount()) + ";";
		}
	}
	LOG_INFO(logString);
}
