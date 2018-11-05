#ifndef INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H
#define INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H

#include "BaseInterprocessDataManager.h"

class IntermediateStorage;

class InterprocessIntermediateStorageManager
	: public BaseInterprocessDataManager
{
public:
	InterprocessIntermediateStorageManager(const std::string& instanceUuid, Id processId, bool isOwner);
	virtual ~InterprocessIntermediateStorageManager() = default;

	void pushIntermediateStorage(const std::shared_ptr<IntermediateStorage>& intermediateStorage);
	std::shared_ptr<IntermediateStorage> popIntermediateStorage();

	size_t getIntermediateStorageCount();

private:
	static const char* s_sharedMemoryNamePrefix;
	static const char* s_intermediatStoragesKeyName;

	size_t m_insertsWithoutGrowth;
};

#endif // INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H
