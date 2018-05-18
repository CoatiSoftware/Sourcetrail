#ifndef SOURCE_GROUP_SETTINGS_H
#define SOURCE_GROUP_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/LanguageType.h"
#include "settings/SourceGroupStatusType.h"
#include "settings/SourceGroupType.h"

class ConfigManager;
class FilePath;
class ProjectSettings;

class SourceGroupSettings
{
public:
	static const size_t s_version;
	static const std::string s_keyPrefix;

	SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettings() = default;

	virtual void load(std::shared_ptr<const ConfigManager> config);
	virtual void save(std::shared_ptr<ConfigManager> config);

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	std::string getId() const;
	void setId(const std::string& id);

	SourceGroupType getType() const;
	LanguageType getLanguage() const;

	std::string getName() const;
	void setName(const std::string& name);

	SourceGroupStatusType getStatus() const;
	void setStatus(SourceGroupStatusType status);

	FilePath getProjectDirectoryPath() const;
	FilePath makePathExpandedAndAbsolute(const FilePath& path) const;
	std::vector<FilePath> makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const;

	virtual std::vector<std::string> getAvailableLanguageStandards() const = 0;

	std::string getStandard() const;
	void setStandard(const std::string& standard);

protected:
	const ProjectSettings* m_projectSettings;

private:
	virtual std::string getDefaultStandard() const = 0;

	std::string m_id;
	std::string m_name;
	const SourceGroupType m_type;
	SourceGroupStatusType m_status;

	std::string m_standard;
};

#endif // SOURCE_GROUP_SETTINGS_H
