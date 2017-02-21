#ifndef SHARED_DICTIONARY_H
#define SHARED_DICTIONARY_H

#include <string>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>

#include "SharedContainer.h"

template<typename Key, typename Value>
class SharedMap : public SharedContainer
{
public:
	SharedMap();
	virtual ~SharedMap();

	virtual void initialize(const bool isOwner, const std::string& mapName);

	Value& operator[](const Key& key);

	unsigned int size() const;

	void clear();

	boost::interprocess::managed_shared_memory::segment_manager* getSegmentManager() const;

private:
	typedef std::pair<Key, Value> ValueType;
	typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;
	typedef boost::interprocess::map<Key, Value, std::less<Key>, ShmemAllocator> ShmemMap;

	ShmemMap* m_map;
};

template<typename Key, typename Value>
SharedMap<Key, Value>::SharedMap()
	: m_map(NULL)
{
}

template<typename Key, typename Value>
SharedMap<Key, Value>::~SharedMap()
{
	if (m_map != NULL && m_initialized && m_isOwner)
	{
		m_sharedMemory.destroy<ShmemDeque>(m_containerName.c_str());
		boost::interprocess::shared_memory_object::remove(m_memoryName.c_str());

		delete m_map;
	}
}

template<typename Key, typename Value>
void SharedMap<Key, Value>::initialize(const bool isOwner, const std::string& mapName)
{
	m_isOwner = isOwner;
	m_containerName = m_containerNamePrefix + dequeName;
	m_memoryName = m_memoryNamePrefix + m_containerName;

	if (initializeSharedMemory(m_isOwner))
	{
		try
		{
			if (m_isOwner)
			{
				m_sharedMemory.destroy<ShmemDeque>(m_containerName.c_str());

				const ShmemAllocator allocator(m_sharedMemory.get_segment_manager());

				m_map = m_sharedMemory.construct<ShmemMap>(m_containerName.c_str())(allocator);
			}
			else
			{
				m_map = m_sharedMemory.find<ShmemMap>(m_containerName.c_str()).first;
			}

			m_initialized = true;
		}
		catch (std::exception& e)
		{
			LOG_ERROR_STREAM(<< e.what());
		}
	}
	
	return m_initialized;
}

template<typename Key, typename Value>
Value& SharedMap<Key, Value>::operator[](const Key& key)
{
	IF_INITIALIZED(NULL)
	{
		return m_map->at(key);
	}
}

template<typename Key, typename Value>
unsigned int SharedMap<Key, Value>::size() const
{
	IF_INITIALIZED(0)
	{
		return m_map->size();
	}
}

template<typename Key, typename Value>
void SharedMap<Key, Value>::clear()
{
	IF_INITIALIZED()
	{
		m_map->clear();
	}
}

template<typename Key, typename Value>
boost::interprocess::managed_shared_memory::segment_manager* SharedMap<Key, Value>::getSegmentManager() const
{
	IF_INITIALIZED(NULL)
	{
		return m_sharedMemory.get_segment_manager();
	}
}

#endif // SHARED_DICTIONARY_H
