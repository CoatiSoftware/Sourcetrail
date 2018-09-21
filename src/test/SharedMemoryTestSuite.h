#include <cxxtest/TestSuite.h>

#include <memory>
#include <thread>

#include "SharedMemory.h"

class SharedMemoryTestSuite : public CxxTest::TestSuite
{
public:
	void test_shared_memory(void)
	{
		SharedMemory memory("memory", 1000, SharedMemory::CREATE_AND_DELETE);

		{
			SharedMemory::ScopedAccess access(&memory);

			TS_ASSERT_EQUALS(access.getMemorySize(), 1000);
			*access.accessValue<int>("count") = 0;
		}

		std::vector<std::shared_ptr<std::thread>> threads;
		for (unsigned int i = 0; i < 4; i++)
		{
			threads.push_back(std::make_shared<std::thread>(
				[]()
				{
					SharedMemory memory("memory", 0, SharedMemory::OPEN_ONLY);

					SharedMemory::ScopedAccess access(&memory);

					if (access.getMemorySize() < 5000)
					{
						access.growMemory(5000 - access.getMemorySize());
					}

					int* count = access.accessValue<int>("count");
					*count += 1;

					SharedMemory::String* str = access.accessValueWithAllocator<SharedMemory::String>("string");
					str->append("hi");

					SharedMemory::Vector<int>* nums = access.accessValueWithAllocator<SharedMemory::Vector<int>>("nums");
					nums->push_back(nums->size());

					SharedMemory::Vector<SharedMemory::String>* strings =
						access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>("strings");
					strings->push_back(SharedMemory::String("ho", access.getAllocator()));

					SharedMemory::Map<int, int>* vals =
						access.accessValueWithAllocator<SharedMemory::Map<int, int>>("vals");
					vals->emplace(vals->size(), vals->size() * vals->size());
				}
			));
		}

		for (auto& thread : threads)
		{
			thread->join();
		}
		threads.clear();

		{
			SharedMemory::ScopedAccess access(&memory);

			TS_ASSERT_EQUALS(access.getMemorySize(), 5000);
			TS_ASSERT_EQUALS(*access.accessValue<int>("count"), 4);
			TS_ASSERT_EQUALS(access.accessValueWithAllocator<SharedMemory::String>("string")->c_str(), "hihihihi");

			SharedMemory::Vector<int>* nums = access.accessValueWithAllocator<SharedMemory::Vector<int>>("nums");
			TS_ASSERT_EQUALS(nums->size(), 4);
			for (int i : *nums)
			{
				TS_ASSERT_EQUALS(i, i);
			}

			SharedMemory::Vector<SharedMemory::String>* strings =
				access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>("strings");
			TS_ASSERT_EQUALS(strings->size(), 4);
			for (SharedMemory::String& str : *strings)
			{
				TS_ASSERT_EQUALS(str, "ho");
			}

			SharedMemory::Map<int, int>* vals =
				access.accessValueWithAllocator<SharedMemory::Map<int, int>>("vals");
			TS_ASSERT_EQUALS(vals->size(), 4);

			size_t i = 0;
			for (auto val : *vals)
			{
				TS_ASSERT_EQUALS(val.first, i);
				TS_ASSERT_EQUALS(val.second, i * i);
				i++;
			}
		}
	}
};
