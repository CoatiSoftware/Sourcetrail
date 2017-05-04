#ifndef INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H
#define INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H

#include "BaseInterprocessDataManager.h"
#include "shared_types/SharedIntermediateStorage.h"

class IntermediateStorage;

class InterprocessIntermediateStorageManager
	: public BaseInterprocessDataManager
{
public:
	InterprocessIntermediateStorageManager(const std::string& instanceUuid, Id processId, bool isOwner);
	virtual ~InterprocessIntermediateStorageManager();

	void pushIntermediateStorage(const std::shared_ptr<IntermediateStorage>& intermediateStorage);
	std::shared_ptr<IntermediateStorage> popIntermediateStorage();

	size_t getIntermediateStorageCount();

private:
	static const char* s_sharedMemoryNamePrefix;
	static const char* s_intermediatStoragesKeyName;
};

#endif // INTERPROCESS_INTERMEDIATE_STORAGE_MANAGER_H
