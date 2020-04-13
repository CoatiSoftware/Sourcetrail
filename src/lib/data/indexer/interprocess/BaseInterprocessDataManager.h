#ifndef BASE_INTERPROCESS_DATA_MANAGER_H
#define BASE_INTERPROCESS_DATA_MANAGER_H

#include <string>

#include "../../../utility/interprocess/SharedMemory.h"
#include "../../../utility/types.h"

class BaseInterprocessDataManager
{
public:
	BaseInterprocessDataManager(
		const std::string& sharedMemoryName,
		size_t initialSharedMemorySize,
		const std::string& instanceUuid,
		Id processId,
		bool isOwner);

	virtual ~BaseInterprocessDataManager() = default;

	Id getProcessId() const;

protected:
	SharedMemory m_sharedMemory;

	const std::string m_instanceUuid;
	const Id m_processId;
};

#endif	  // BASE_INTERPROCESS_DATA_MANAGER_H
