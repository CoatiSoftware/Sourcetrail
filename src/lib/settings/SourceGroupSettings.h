#ifndef SOURCE_GROUP_SETTINGS_H
#define SOURCE_GROUP_SETTINGS_H

#include <memory>
#include <vector>

#include "LanguageType.h"
#include "SourceGroupSettingsBase.h"
#include "SourceGroupStatusType.h"
#include "SourceGroupType.h"

class ConfigManager;
class FilePath;

class SourceGroupSettings
	: virtual public SourceGroupSettingsBase
{
public:
	static const size_t s_version;
	static const std::string s_keyPrefix;

	SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettings() = default;

	virtual std::shared_ptr<SourceGroupSettings> createCopy() const = 0;

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

	const ProjectSettings* getProjectSettings() const override;
	FilePath getProjectDirectoryPath() const;

	FilePath makePathExpandedAndAbsolute(const FilePath& path) const;
	std::vector<FilePath> makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const;

protected:
	const ProjectSettings* m_projectSettings;

private:
	std::string m_id;
	std::string m_name;
	const SourceGroupType m_type;
	SourceGroupStatusType m_status;
};

#endif // SOURCE_GROUP_SETTINGS_H
