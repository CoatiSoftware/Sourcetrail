#include "SharedMemory.h"

#include "SharedMemoryGarbageCollector.h"
#include "logging.h"

const char* SharedMemory::s_memoryNamePrefix = "srctrlmem_";
const char* SharedMemory::s_mutexNamePrefix = "srctrlmtx_";

SharedMemory::ScopedAccess::ScopedAccess(SharedMemory* memory)
	: boost::interprocess::scoped_lock<boost::interprocess::named_mutex>(memory->getMutex())
	, m_memory(boost::interprocess::open_only, memory->getMemoryName().c_str())
	, m_memoryName(memory->getMemoryName())
	, m_minimumMemorySize(memory->getInitialMemorySize())
{
}

SharedMemory::ScopedAccess::~ScopedAccess() {}

SharedMemory::Allocator* SharedMemory::ScopedAccess::getAllocator()
{
	return m_memory.get_segment_manager();
}

size_t SharedMemory::ScopedAccess::getMemorySize() const
{
	return m_memory.get_size();
}

size_t SharedMemory::ScopedAccess::getFreeMemorySize() const
{
	return m_memory.get_free_memory();
}

size_t SharedMemory::ScopedAccess::getUsedMemorySize() const
{
	return getMemorySize() - getFreeMemorySize();
}

void SharedMemory::ScopedAccess::growMemory(size_t size)
{
	m_memory = boost::interprocess::managed_shared_memory();

	boost::interprocess::managed_shared_memory::grow(m_memoryName.c_str(), size);

	m_memory = boost::interprocess::managed_shared_memory(
		boost::interprocess::open_only, m_memoryName.c_str());
}

void SharedMemory::ScopedAccess::shrinkToFitMemory()
{
	if (getUsedMemorySize() < m_minimumMemorySize)
	{
		return;
	}

	m_memory = boost::interprocess::managed_shared_memory();

	boost::interprocess::managed_shared_memory::shrink_to_fit(m_memoryName.c_str());

	m_memory = boost::interprocess::managed_shared_memory(
		boost::interprocess::open_only, m_memoryName.c_str());
}

std::string SharedMemory::ScopedAccess::logString() const
{
	std::string log = m_memoryName + " -";
	log += " size: " + std::to_string(getMemorySize());
	log += " free: " + std::to_string(getFreeMemorySize());
	log += " used: " + std::to_string(getUsedMemorySize());
	log += " used pct: " +
		std::to_string(100 - int(float(getFreeMemorySize()) / getMemorySize() * 100));
	return log;
}


std::string SharedMemory::checkName(const std::string& name)
{
	return name.size() > 18 ? name.substr(0, 18) : name;
}

std::string SharedMemory::checkSharedMemory(const std::string& name)
{
	std::string error;

	try
	{
		SharedMemory memory("test_" + name, 65536 /* 64 kB */, CREATE_AND_DELETE);
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR_STREAM(<< "boost exception thrown at shared memory check: " << e.what());
		error = e.what();
	}

	return error;
}

void SharedMemory::deleteSharedMemory(const std::string& name)
{
	boost::interprocess::shared_memory_object::remove((s_memoryNamePrefix + name).c_str());
	boost::interprocess::named_mutex::remove((s_mutexNamePrefix + name).c_str());
}

SharedMemory::SharedMemory(const std::string& name, size_t initialMemorySize, AccessMode mode)
	: m_name(checkName(name)), m_mode(mode), m_initialMemorySize(initialMemorySize)
{
	try
	{
		bool unlockMutex = true;

		switch (mode)
		{
		case CREATE_AND_DELETE:
		{
			SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::getInstance();
			if (collector)
			{
				collector->registerSharedMemory(m_name);
			}

			deleteSharedMemory(m_name);

			boost::interprocess::permissions permissions;
			permissions.set_unrestricted();

			boost::interprocess::managed_shared_memory(
				boost::interprocess::create_only,
				getMemoryName().c_str(),
				initialMemorySize,
				0,
				permissions);
			boost::interprocess::named_mutex(
				boost::interprocess::create_only, getMutexName().c_str());
		}
		break;

		case OPEN_ONLY:
			boost::interprocess::managed_shared_memory(
				boost::interprocess::open_only, getMemoryName().c_str());
			boost::interprocess::named_mutex(boost::interprocess::open_only, getMutexName().c_str());
			unlockMutex = false;
			break;

		case OPEN_OR_CREATE:
		{
			boost::interprocess::permissions permissions;
			permissions.set_unrestricted();

			boost::interprocess::managed_shared_memory(
				boost::interprocess::open_or_create,
				getMemoryName().c_str(),
				initialMemorySize,
				0,
				permissions);
			boost::interprocess::named_mutex(
				boost::interprocess::open_or_create, getMutexName().c_str());
		}
		break;
		}

		if (unlockMutex)
		{
			boost::interprocess::named_mutex mutex(
				boost::interprocess::open_only, getMutexName().c_str());
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(
				mutex, boost::interprocess::try_to_lock);
		}
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR_STREAM(
			<< "boost exception thrown at shared memory creation - " << getMemoryName() << ": "
			<< e.what());
		throw e;
	}
}

SharedMemory::~SharedMemory()
{
	try
	{
		if (m_mode == CREATE_AND_DELETE)
		{
			SharedMemoryGarbageCollector* collector = SharedMemoryGarbageCollector::getInstance();
			if (collector)
			{
				collector->unregisterSharedMemory(m_name);
			}

			deleteSharedMemory(m_name);
		}
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR_STREAM(
			<< "boost exception thrown at shared memory destruction - " << getMemoryName() << ": "
			<< e.what());
	}
}

bool SharedMemory::checkSharedMutex()
{
	try
	{
		boost::interprocess::named_mutex& mutex = getMutex();

		for (size_t i = 0; i < 5; i++)
		{
			{
				// try to get ownership of the mutex a couple times
				boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(
					mutex, boost::interprocess::try_to_lock);
				if (lock.owns())	// mutex successfully locked
				{
					return true;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

		// locking kept failing, try to get ownership
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(
			mutex, boost::interprocess::accept_ownership);
		return true;
	}
	catch (boost::interprocess::interprocess_exception& e)
	{
		LOG_ERROR_STREAM(<< "boost exception thrown at shared mutex check: " << e.what());
	}

	return false;
}

std::string SharedMemory::getMemoryName() const
{
	return s_memoryNamePrefix + m_name;
}

std::string SharedMemory::getMutexName() const
{
	return s_mutexNamePrefix + m_name;
}

boost::interprocess::named_mutex& SharedMemory::getMutex()
{
	if (!m_mutex)
	{
		m_mutex = std::make_shared<boost::interprocess::named_mutex>(
			boost::interprocess::open_only, getMutexName().c_str());
	}

	return *m_mutex.get();
}

size_t SharedMemory::getInitialMemorySize() const
{
	return m_initialMemorySize;
}
