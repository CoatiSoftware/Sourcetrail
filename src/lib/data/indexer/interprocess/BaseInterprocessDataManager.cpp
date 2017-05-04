#include "BaseInterprocessDataManager.h"

BaseInterprocessDataManager::BaseInterprocessDataManager(
	const std::string& sharedMemoryName,
	size_t initialSharedMemorySize,
	const std::string& instanceUuid,
	Id processId,
	bool isOwner
)
	: m_sharedMemory(sharedMemoryName, initialSharedMemorySize, isOwner ? SharedMemory::CREATE_AND_DELETE : SharedMemory::OPEN_ONLY)
	, m_instanceUuid(instanceUuid)
	, m_processId(processId)
{
}

BaseInterprocessDataManager::~BaseInterprocessDataManager()
{
}

Id BaseInterprocessDataManager::getProcessId() const
{
	return m_processId;
}
