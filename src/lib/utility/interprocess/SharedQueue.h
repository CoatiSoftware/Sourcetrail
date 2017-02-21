#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H

#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include "SharedContainer.h"

template<typename T>
class SharedQueue : public SharedContainer
{
public:
	SharedQueue();
	virtual ~SharedQueue();

	virtual bool initialize(const bool isOwner, const std::string& dequeName);

	void pushValue(const T& val);
	T popValue();

	unsigned int size() const;

	boost::interprocess::managed_shared_memory::segment_manager* getSegmentManager() const;

private:	
	typedef boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;
	typedef boost::interprocess::deque<T, ShmemAllocator> ShmemDeque;

	ShmemDeque* m_deque;
};

template<typename T>
SharedQueue<T>::SharedQueue()
	: m_deque(NULL)
{
}

template<typename T>
SharedQueue<T>::~SharedQueue()
{
	if (m_deque != NULL && m_initialized && m_isOwner)
	{
		m_sharedMemory.destroy<ShmemDeque>(m_containerName.c_str());
		boost::interprocess::shared_memory_object::remove(m_memoryName.c_str());

		delete m_deque;
	}
}

template<typename T>
bool SharedQueue<T>::initialize(const bool isOwner, const std::string& dequeName)
{
	m_isOwner = isOwner;
	m_containerName = m_containerNamePrefix + dequeName;
	m_memoryName = m_memoryNamePrefix + m_containerName;

	if (initializeSharedMemory(m_isOwner))
	{
		try
		{
			if (isOwner)
			{
				m_sharedMemory.destroy<ShmemDeque>(m_containerName.c_str());

				const ShmemAllocator allocator(m_sharedMemory.get_segment_manager());

				m_deque = m_sharedMemory.construct<ShmemDeque>(m_containerName.c_str())(allocator);
			}
			else
			{
				m_deque = m_sharedMemory.find<ShmemDeque>(m_containerName.c_str()).first;
			}

			m_initialized = true;
		}
		catch (std::exception& e)
		{
			LOG_ERROR(e.what());
		}
	}

	return m_initialized;
}

template<typename T>
void SharedQueue<T>::pushValue(const T& val)
{
	IF_INITIALIZED()
	{
		m_deque->push_back(val);
	}
}

template<typename T>
T SharedQueue<T>::popValue()
{
	if (m_initialized)
	{
		T val = m_deque->front();
		m_deque->pop_front();

		return val;
	}
	else
	{
		throw(std::exception("Deque was not initialized"));
	}
}

template<typename T>
unsigned int SharedQueue<T>::size() const
{
	IF_INITIALIZED(0)
	{
		return m_deque->size();
	}
}

template<typename T>
boost::interprocess::managed_shared_memory::segment_manager* SharedQueue<T>::getSegmentManager() const
{
	IF_INITIALIZED(NULL)
	{
		return m_sharedMemory.get_segment_manager();
	}
}

#endif // SHARED_QUEUE_H
