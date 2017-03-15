#include "SharedUUIDManager.h"

#include <fstream>

#include "utility/file/FileSystem.h"
#include "utility/ConfigManager.h"
#include "utility/text/TextAccess.h"

#include "utility/UserPaths.h"

const std::string SharedUUIDManager::m_fileName("sharedUUIDs.xml");
const std::string SharedUUIDManager::m_instanceUUIDsKey("instances");
std::shared_ptr<SharedUUIDManager> SharedUUIDManager::m_instance(NULL);

std::shared_ptr<SharedUUIDManager> SharedUUIDManager::getInstance()
{
	if (m_instance == NULL)
	{
		// m_instance = std::make_shared<SharedUUIDManager>();
		SharedUUIDManager* sharedUUIDManager = new SharedUUIDManager();
		m_instance = std::shared_ptr<SharedUUIDManager>(sharedUUIDManager);
		m_instance->setFilePath(UserPaths::getUserDataPath());
		m_instance->saveInstanceUUID();
	}

	return m_instance;
}

SharedUUIDManager::~SharedUUIDManager()
{
}

void SharedUUIDManager::setFilePath(const std::string& filePath)
{
	m_filePath = filePath;
	FileSystem::createDirectory(m_filePath);

	refreshUUIDs();
}

std::string SharedUUIDManager::getInstanceUUID() const
{
	return UUIDUtility::UUIDtoString(m_instanceUUID);
}

std::string SharedUUIDManager::getNewUUID()
{
	std::string uuidString = UUIDUtility::getUUIDString();

	std::vector<std::string> uuids;
	m_uuids->getValues(UUIDUtility::UUIDtoString(m_instanceUUID), uuids);

	uuids.push_back(uuidString);

	m_uuids->setValues(UUIDUtility::UUIDtoString(m_instanceUUID), uuids);

	m_uuids->save(m_filePath + m_fileName);

	return uuidString;
}

std::vector<std::string> SharedUUIDManager::getUUIDsForInstance(const std::string& instanceUUID)
{
	std::vector<std::string> uuids;
	m_uuids->getValues(instanceUUID, uuids);

	return uuids;
}

void SharedUUIDManager::removeUUIDsForInstance(const std::string& instanceUUID)
{
	m_uuids->removeValues(instanceUUID);

	m_uuids->save(m_filePath + m_fileName);
}

std::vector<std::string> SharedUUIDManager::getStoredInstanceUUIDs() const
{
	std::vector<std::string> instanceUUIDs;
	m_uuids->getValues(m_instanceUUIDsKey, instanceUUIDs);

	return instanceUUIDs;
}

void SharedUUIDManager::removeInstanceUUID(const std::string& instanceUUID)
{
	std::vector<std::string> instanceUUIDs;
	m_uuids->getValues(m_instanceUUIDsKey, instanceUUIDs);

	// instanceUUIDs
	for (std::vector<std::string>::iterator it = instanceUUIDs.begin(); it != instanceUUIDs.end(); it++)
	{
		if (*it == instanceUUID)
		{
			instanceUUIDs.erase(it);
			break;
		}
	}

	m_uuids->setValues(m_instanceUUIDsKey, instanceUUIDs);
	m_uuids->save(m_filePath + m_fileName);
}

SharedUUIDManager::SharedUUIDManager()
	: m_filePath("user/")
	, m_instanceUUID(UUIDUtility::getUUID())
	, m_uuids(NULL)
{
}

void SharedUUIDManager::saveInstanceUUID()
{
	std::vector<std::string> instanceUUIDs;
	m_uuids->getValues(m_instanceUUIDsKey, instanceUUIDs);

	instanceUUIDs.push_back(getInstanceUUID());
	m_uuids->setValues(m_instanceUUIDsKey, instanceUUIDs);
}

void SharedUUIDManager::refreshUUIDs()
{
	if (FilePath(m_filePath + m_fileName).exists())
	{
		m_uuids = ConfigManager::createAndLoad(TextAccess::createFromFile(m_filePath + m_fileName));
	}
}
