#ifndef SHARED_MEMORY_GARBAGE_COLLECTOR_H
#define SHARED_MEMORY_GARBAGE_COLLECTOR_H

#include <memory>
#include <mutex>
#include <string>
#include <set>

#include "utility/interprocess/SharedMemory.h"

#include <mutex>

class SharedMemoryGarbageCollector
{
public:
	static SharedMemoryGarbageCollector* createInstance();
	static SharedMemoryGarbageCollector* getInstance();

	SharedMemoryGarbageCollector();
	~SharedMemoryGarbageCollector();

	void run(const std::string& uuid);
	void stop();

	void registerSharedMemory(const std::string& sharedMemoryName);
	void unregisterSharedMemory(const std::string& sharedMemoryName);

private:
	void update();

	static std::string s_memoryName;
	static std::string s_instancesKeyName;
	static std::string s_timeStampsKeyName;

	static const size_t s_updateIntervalSeconds;
	static const size_t s_deleteThresholdSeconds;

	static std::shared_ptr<SharedMemoryGarbageCollector> s_instance;

	SharedMemory m_memory;
	volatile bool m_loopIsRunning;
	std::shared_ptr<std::thread> m_thread;

	std::string m_uuid;

	std::mutex m_sharedMemoryNamesMutex;
	std::set<std::string> m_sharedMemoryNames;
	std::set<std::string> m_removedSharedMemoryNames;
};

#endif // SHARED_MEMORY_GARBAGE_COLLECTOR_H
