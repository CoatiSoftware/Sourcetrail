#ifndef SHARED_UUID_MANAGER_H
#define SHARED_UUID_MANAGER_H

#include <string>
#include <memory>

#include "utility/UUIDUtility.h"

#include "utility/ConfigManager.h"

class SharedUUIDManager
{
public:
	static std::shared_ptr<SharedUUIDManager> getInstance();

	~SharedUUIDManager();

	void setFilePath(const std::string& filePath);

	std::string getInstanceUUID() const;
	std::string getNewUUID();

	std::vector<std::string> getUUIDsForInstance(const std::string& instanceUUID);
	void removeUUIDsForInstance(const std::string& instanceUUID);

	std::vector<std::string> getStoredInstanceUUIDs() const;
	void removeInstanceUUID(const std::string& instanceUUID);

private:
	SharedUUIDManager();

	void saveInstanceUUID();

	void refreshUUIDs();

	static const std::string m_fileName;
	static const std::string m_instanceUUIDsKey;
	static std::shared_ptr<SharedUUIDManager> m_instance;

	std::string m_filePath;

	const UUID m_instanceUUID;

	std::shared_ptr<ConfigManager> m_uuids;
};

#endif // SHARED_UUID_MANAGER_H
