#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <string>

#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

class SharedMemory
{
public:
	enum AccessMode
	{
		CREATE_AND_DELETE,
		OPEN_ONLY,
		OPEN_OR_CREATE
	};

	using Allocator = boost::interprocess::managed_shared_memory::segment_manager;

	using String = boost::interprocess::basic_string<char, std::char_traits<char>,
		boost::interprocess::allocator<char, Allocator>>;

	template <typename T>
	using Vector = boost::interprocess::vector<T, boost::interprocess::allocator<T, Allocator>>;

	template <typename T>
	using Queue = boost::interprocess::deque<T, boost::interprocess::allocator<T, Allocator>>;

	template <typename T, typename T2>
	using Map = boost::interprocess::map<T, T2, std::less<T>,
		boost::interprocess::allocator<std::pair<const T, T2>, Allocator>>;

	template <typename T>
	using Set = boost::interprocess::set<T, std::less<T>, boost::interprocess::allocator<T, Allocator>>;

	// Names addressing shared memory objects longer than 29 characters can throw an exception
	static std::string checkName(const std::string& name);
	static std::string checkSharedMemory(const std::string& name);
	static void deleteSharedMemory(const std::string& name);

	SharedMemory(const std::string& name, size_t initialMemorySize, AccessMode mode);
	~SharedMemory();

	class ScopedAccess
		: public boost::interprocess::scoped_lock<boost::interprocess::named_mutex>
	{
	public:
		ScopedAccess(SharedMemory* memory);
		~ScopedAccess();

		Allocator* getAllocator();

		size_t getMemorySize() const;
		size_t getFreeMemorySize() const;
		size_t getUsedMemorySize() const;

		void growMemory(size_t size);

		template <typename T>
		T* accessValue(const std::string& key)
		{
			return m_memory.find_or_construct<T>(key.c_str())();
		}

		template <typename T>
		T* accessValues(const std::string& key, size_t count)
		{
			return m_memory.find_or_construct<T>(key.c_str())[count]();
		}

		template <typename T>
		T* accessValueWithAllocator(const std::string& key)
		{
			return m_memory.find_or_construct<T>(key.c_str())(getAllocator());
		}

		template <typename T>
		T* accessValuesWithAllocator(const std::string& key, size_t count)
		{
			return m_memory.find_or_construct<T>(key.c_str())[count](getAllocator());
		}

		template <typename T>
		void destroyValue(const std::string& key)
		{
			m_memory.destroy<T>(key.c_str());
		}

		std::string logString() const;

	private:
		boost::interprocess::managed_shared_memory m_memory;
		std::string m_memoryName;
	};

	void unlockSharedMutex();

private:
	static const char* s_memoryNamePrefix;
	static const char* s_mutexNamePrefix;

	std::string getMemoryName() const;
	std::string getMutexName() const;

	boost::interprocess::named_mutex& getMutex();

	std::shared_ptr<boost::interprocess::named_mutex> m_mutex;
	std::string m_name;
	AccessMode m_mode;
};


#endif // SHARED_MEMORY_H
