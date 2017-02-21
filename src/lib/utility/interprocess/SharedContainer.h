#ifndef SHARED_CONTAINER_H
#define SHARED_CONTAINER_H

#include <string>

#include <boost/interprocess/managed_shared_memory.hpp>

#include "utility/logging/logging.h"

#include "InterprocessUtility.h"

class SharedContainer
{
public:
	static std::string getMemoryNamePrefix();
	static std::string getContainerNamePrefix();

	SharedContainer();
	virtual ~SharedContainer();

	virtual bool initialize(const bool isOwner, const std::string& containerName) = 0;

protected:
	static const std::string m_memoryNamePrefix;
	static const std::string m_containerNamePrefix;

	bool initializeSharedMemory(const bool isOwner);

	boost::interprocess::managed_shared_memory m_sharedMemory;

	bool m_isOwner;
	bool m_initialized;

	std::string m_memoryName;
	std::string m_containerName;
};

const std::string SharedContainer::m_memoryNamePrefix("coati_memory_");

const std::string SharedContainer::m_containerNamePrefix("coati_container_");

std::string SharedContainer::getMemoryNamePrefix()
{
	return m_memoryNamePrefix;
}

std::string SharedContainer::getContainerNamePrefix()
{
	return m_containerNamePrefix;
}

SharedContainer::SharedContainer()
	: m_isOwner(false)
	, m_initialized(false)
	, m_memoryName(m_memoryNamePrefix)
	, m_containerName(m_containerNamePrefix)
{
}

SharedContainer::~SharedContainer()
{
}

bool SharedContainer::initializeSharedMemory(const bool isOwner)
{
	bool initialized = false;

	try
	{
		if (isOwner)
		{
			boost::interprocess::shared_memory_object::remove(m_memoryName.c_str());

			m_sharedMemory = boost::interprocess::managed_shared_memory(boost::interprocess::create_only,
				m_memoryName.c_str(),
				65536);
		}
		else
		{
			m_sharedMemory = boost::interprocess::managed_shared_memory(boost::interprocess::open_only,
				m_memoryName.c_str());
		}

		initialized = true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR(e.what());
	}

	return initialized;
}


#endif // SHARED_CONTAINER_H
