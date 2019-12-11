#include "catch.hpp"

#include <memory>
#include <thread>

#include "SharedMemory.h"

TEST_CASE("shared memory")
{
	SharedMemory memory("memory", 1000, SharedMemory::CREATE_AND_DELETE);

	{
		SharedMemory::ScopedAccess access(&memory);

		REQUIRE(access.getMemorySize() == 1000);
		*access.accessValue<int>("count") = 0;
	}

	std::vector<std::shared_ptr<std::thread>> threads;
	for (unsigned int i = 0; i < 4; i++)
	{
		threads.push_back(std::make_shared<std::thread>([]() {
			SharedMemory memory("memory", 0, SharedMemory::OPEN_ONLY);

			SharedMemory::ScopedAccess access(&memory);

			if (access.getMemorySize() < 5000)
			{
				access.growMemory(5000 - access.getMemorySize());
			}

			int* count = access.accessValue<int>("count");
			*count += 1;

			SharedMemory::String* str = access.accessValueWithAllocator<SharedMemory::String>(
				"string");
			str->append("hi");

			SharedMemory::Vector<int>* nums =
				access.accessValueWithAllocator<SharedMemory::Vector<int>>("nums");
			nums->push_back(static_cast<int>(nums->size()));

			SharedMemory::Vector<SharedMemory::String>* strings =
				access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(
					"strings");
			strings->push_back(SharedMemory::String("ho", access.getAllocator()));

			SharedMemory::Map<int, int>* vals =
				access.accessValueWithAllocator<SharedMemory::Map<int, int>>("vals");
			vals->emplace(
				static_cast<int>(vals->size()), static_cast<int>(vals->size() * vals->size()));
		}));
	}

	for (auto& thread: threads)
	{
		thread->join();
	}
	threads.clear();

	{
		SharedMemory::ScopedAccess access(&memory);

		REQUIRE(access.getMemorySize() == 5000);
		REQUIRE(*access.accessValue<int>("count") == 4);
		const std::string value =
			access.accessValueWithAllocator<SharedMemory::String>("string")->c_str();
		REQUIRE(value == "hihihihi");

		SharedMemory::Vector<int>* nums = access.accessValueWithAllocator<SharedMemory::Vector<int>>(
			"nums");
		REQUIRE(nums->size() == 4);
		for (int i: *nums)
		{
			REQUIRE(i == i);
		}

		SharedMemory::Vector<SharedMemory::String>* strings =
			access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>("strings");
		REQUIRE(strings->size() == 4);
		for (SharedMemory::String& str: *strings)
		{
			REQUIRE(str == "ho");
		}

		SharedMemory::Map<int, int>* vals =
			access.accessValueWithAllocator<SharedMemory::Map<int, int>>("vals");
		REQUIRE(vals->size() == 4);

		size_t i = 0;
		for (auto val: *vals)
		{
			REQUIRE(val.first == i);
			REQUIRE(val.second == i * i);
			i++;
		}
	}
}
