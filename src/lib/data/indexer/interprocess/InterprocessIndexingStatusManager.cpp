#include "InterprocessIndexingStatusManager.h"

#include "utility/logging/logging.h"

const char* InterprocessIndexingStatusManager::s_sharedMemoryNamePrefix = "ists_";

const char* InterprocessIndexingStatusManager::s_lastFileKeyName = "last_file";
const char* InterprocessIndexingStatusManager::s_currentFilesKeyName = "current_files";
const char* InterprocessIndexingStatusManager::s_crashedFilesKeyName = "crashed_files";
const char* InterprocessIndexingStatusManager::s_indexedFilesKeyName = "indexed_files";

InterprocessIndexingStatusManager::InterprocessIndexingStatusManager(const std::string& instanceUuid, Id processId, bool isOwner)
	: BaseInterprocessDataManager(s_sharedMemoryNamePrefix + instanceUuid, 1048576 /* 1 MB */, instanceUuid, processId, isOwner)
{
}

InterprocessIndexingStatusManager::~InterprocessIndexingStatusManager()
{
}

void InterprocessIndexingStatusManager::setCurrentlyIndexedSourceFilePath(const FilePath& filePath)
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::String* strPtr = access.accessValueWithAllocator<SharedMemory::String>(s_lastFileKeyName);
	if (strPtr)
	{
		*strPtr = filePath.str().c_str();
	}

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		SharedMemory::Map<Id, SharedMemory::String>::iterator it = currentFilesPtr->find(getProcessId());
		if (it != currentFilesPtr->end())
		{
			SharedMemory::Vector<SharedMemory::String>* crashedFilesPtr =
				access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(s_crashedFilesKeyName);

			if (crashedFilesPtr)
			{
				crashedFilesPtr->push_back(it->second);
			}
		}

		SharedMemory::String str(access.getAllocator());
		str = filePath.str().c_str();

		it = currentFilesPtr->insert(std::pair<Id, SharedMemory::String>(getProcessId(), str)).first;
		it->second = str;
	}
}

void InterprocessIndexingStatusManager::clearCurrentlyIndexedSourceFilePath()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		currentFilesPtr->erase(currentFilesPtr->find(getProcessId()), currentFilesPtr->end());
	}
}

FilePath InterprocessIndexingStatusManager::getCurrentlyIndexedSourceFilePath()
{
	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::String* strPtr = access.accessValueWithAllocator<SharedMemory::String>(s_lastFileKeyName);
	if (strPtr)
	{
		return FilePath(strPtr->c_str());
	}

	return FilePath();
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
			crashedFiles.push_back(FilePath(crashedFilesPtr->at(i).c_str()));
		}
	}

	SharedMemory::Map<Id, SharedMemory::String>* currentFilesPtr =
		access.accessValueWithAllocator<SharedMemory::Map<Id, SharedMemory::String>>(s_currentFilesKeyName);
	if (currentFilesPtr)
	{
		for (SharedMemory::Map<Id, SharedMemory::String>::iterator it = currentFilesPtr->begin(); it != currentFilesPtr->end(); it++)
		{
			crashedFiles.push_back(FilePath(it->second.c_str()));
		}
	}

	return crashedFiles;
}

std::set<FilePath> InterprocessIndexingStatusManager::getIndexedFiles()
{
	std::set<FilePath> result;

	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Vector<SharedMemory::String>* files =
		access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(s_indexedFilesKeyName);
	if (!files)
	{
		return result;
	}

	for (auto file : *files)
	{
		result.insert(FilePath(file.c_str()));
	}

	return result;
}

void InterprocessIndexingStatusManager::addIndexedFiles(std::set<FilePath> filePaths)
{
	const unsigned int overestimationMultiplier = 3;

	SharedMemory::ScopedAccess access(&m_sharedMemory);

	SharedMemory::Vector<SharedMemory::String>* files =
		access.accessValueWithAllocator<SharedMemory::Vector<SharedMemory::String>>(s_indexedFilesKeyName);
	if (!files)
	{
		return;
	}

	for (auto file : *files)
	{
		filePaths.insert(FilePath(file.c_str()));
	}
	files->clear();


	size_t size = 1000;
	for (auto path : filePaths)
	{
		size += sizeof(std::string) + path.str().size();
	}
	size *= overestimationMultiplier;

	size_t freeMemory = access.getFreeMemorySize();
	if (freeMemory <= size)
	{
		LOG_INFO_STREAM(
			<< "grow memory - est: " << size << " size: " << access.getMemorySize()
			<< " free: " << access.getFreeMemorySize() << " alloc: " << (size - freeMemory));
		access.growMemory(size - freeMemory);
	}

	for (auto path : filePaths)
	{
		files->push_back(SharedMemory::String(path.str().c_str(), access.getAllocator()));
	}

	LOG_INFO(access.logString());
}
