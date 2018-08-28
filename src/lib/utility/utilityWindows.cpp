#include "utility/utilityWindows.h"

#ifdef WIN32

#include <Windows.h>

unsigned long utility::getLargestByteSizeOfAllocatableMemory()
{
	MEMORY_BASIC_INFORMATION mbi;
	unsigned long start = 0;
	bool recording = false;
	unsigned long freestart = 0, largestFreestart = 0;
	__int64 free = 0, largestFree = 0;

	while (true)
	{
		SIZE_T s = VirtualQuery((LPCVOID)start, &mbi, sizeof(mbi));
		if (s != sizeof(mbi)) break;

		if (mbi.State == MEM_FREE)
		{
			if (!recording) freestart = start;

			free += mbi.RegionSize;
			recording = true;
		}
		else
		{
			if (recording)
			{
				if (free > largestFree)
				{
					largestFree = free;
					largestFreestart = freestart;
				}
			}
			free = 0;
			recording = false;
		}
		start += mbi.RegionSize;
	}

	return largestFree;
}

#endif // WIN32
