#include "InterprocessIndexingStatusManager.h"

#include "logging.h"
#include "utilityString.h"

const char* InterprocessIndexingStatusManager::s_sharedMemoryNamePrefix = "ists_";

const char* InterprocessIndexingStatusManager::s_indexingFilesKeyName = "indexing_files";
const char* InterprocessIndexingStatusManager::s_currentFilesKeyName = "current_files";
const char* InterprocessIndexingStatusManager::s_crashedFilesKeyName = "crashed_files";
const char* InterprocessIndexingStatusManager::s_finishedProcessIdsKeyName = "finished_process_ids";
const char* InterprocessIndexingStatusManager::s_indexingInterruptedKeyName = "indexing_interrupted_flag";

InterprocessIndexingStatusManager::InterprocessIndexingStatusManager(const std::string& instanceUuid, Id processId, bool isOwner)
	: BaseInterprocessDataManager(s_sharedMemoryNamePrefix + instanceUuid, 1048576 /* 1 MB */, instanceUuid, processId, isOwner)
{
}

InterprocessIndexingStatusManager::~InterprocessIndexingStatusManager()
{
}

void InterprocessIndexingStatusManager::startIndexingSourceFile(const FilePath& filePath)
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<SharedMemory::String>* indexingFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedMemory::String>>(s_indexingFilesKeyName);
	if (indexingFilesPtr)
	{
		SharedMemory::String fileStr(access.getAllocator());
		fileStr = utility::encodeToUtf8(filePath.wstr()).c_str();
		indexingFilesPtr->push_back(fileStr);
	}

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		SharedMemory::Map<Id, SharedMemory::String>::iterator it = currentFilesPtr->find(getProcessId());
		if (it != currentFilesPtr->end())
		{
			const size_t overestimationMultiplier = 3;
			const std::string crashedFilePath = it->second.c_str();

			size_t estimatedSize = 262144 + sizeof(SharedMemory::String) + crashedFilePath.size();
			estimatedSize *= overestimationMultiplier;

			while (access.getFreeMemorySize() < estimatedSize)
			{
				LOG_INFO_STREAM(
					<< "grow memory - est: " << estimatedSize << " size: " << access.getMemorySize()
					<< " free: " << access.getFreeMemorySize() << " alloc: " << (access.getMemorySize()));
				access.growMemory(access.getMemorySize());

				LOG_INFO("growing memory succeeded");

				currentFilesPtr = access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
				if (!currentFilesPtr)
				{
					return;
				}
			}

			SharedMemory::Vector<SharedMemory::String>* crashedFilesPtr =
				access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(s_crashedFilesKeyName);

			if (crashedFilesPtr)
			{
				crashedFilesPtr->push_back(it->second);
			}
		}

		SharedMemory::String str(access.getAllocator());
		str = utility::encodeToUtf8(filePath.wstr()).c_str();

		it = currentFilesPtr->insert(std::pair<Id, SharedMemory::String>(getProcessId(), str)).first;
		it->second = str;
	}
}

void InterprocessIndexingStatusManager::finishIndexingSourceFile()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		currentFilesPtr->erase(currentFilesPtr->find(getProcessId()), currentFilesPtr->end());
	}

	SharedMemory::Queue<Id>* finishedProcessIdsPtr =
		access.accessValueWithAllocator<SharedMemory::Queue<Id>>(s_finishedProcessIdsKeyName);
	if (finishedProcessIdsPtr)
	{
		finishedProcessIdsPtr->push_back(m_processId);
	}
}

void InterprocessIndexingStatusManager::setIndexingInterrupted(bool interrupted)
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	bool* indexingInterruptedPtr =
		access.accessValue<bool>(s_indexingInterruptedKeyName);
	if (indexingInterruptedPtr)
	{
		*indexingInterruptedPtr = interrupted;
	}
}

bool InterprocessIndexingStatusManager::getIndexingInterrupted()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	bool* indexingInterruptedPtr =
		access.accessValue<bool>(s_indexingInterruptedKeyName);
	if (indexingInterruptedPtr)
	{
		return *indexingInterruptedPtr;
	}

	return false;
}

Id InterprocessIndexingStatusManager::getNextFinishedProcessId()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Queue<Id>* finishedProcessIdsPtr =
		access.accessValueWithAllocator<SharedMemory::Queue<Id>>(s_finishedProcessIdsKeyName);
	if (finishedProcessIdsPtr && finishedProcessIdsPtr->size())
	{
		Id processId = finishedProcessIdsPtr->front();
		finishedProcessIdsPtr->pop_front();
		return processId;
	}

	return 0;
}

std::vector<FilePath> InterprocessIndexingStatusManager::getCurrentlyIndexedSourceFilePaths()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	std::vector<FilePath> indexingFiles;

	SharedMemory::Queue<SharedMemory::String>* indexingFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Queue<SharedMemory::String>>(s_indexingFilesKeyName);
	if (indexingFilesPtr)
	{
		while (indexingFilesPtr->size())
		{
			indexingFiles.push_back(FilePath(utility::decodeFromUtf8(indexingFilesPtr->front().c_str())));
			indexingFilesPtr->pop_front();
		}
	}

	return indexingFiles;
}

std::vector<FilePath> InterprocessIndexingStatusManager::getCrashedSourceFilePaths()
{
	std::vector<FilePath> crashedFiles;

	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Vector<SharedMemory::String>* crashedFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(s_crashedFilesKeyName);

	if (crashedFilesPtr)
	{
		for (size_t i = 0; i < crashedFilesPtr->size(); i++)
		{
			crashedFiles.push_back(FilePath(utility::decodeFromUtf8(crashedFilesPtr->at(i).c_str())));
		}
	}

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		for (SharedMemory::Map<Id, SharedMemory::String>::iterator it = currentFilesPtr->begin(); it != currentFilesPtr->end(); it++)
		{
			crashedFiles.push_back(FilePath(utility::decodeFromUtf8(it->second.c_str())));
		}
	}

	return crashedFiles;
}
