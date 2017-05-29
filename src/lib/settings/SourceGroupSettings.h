#ifndef SOURCE_GROUP_SETTINGS_H
#define SOURCE_GROUP_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/ProjectSettings.h"
#include "settings/SourceGroupType.h"

class ProjectSettings;

class SourceGroupSettings
{
public:
	SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettings();

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	std::string getId() const;
	void setId(const std::string& id);

	std::string getName() const;
	void setName(const std::string& name);

	FilePath getProjectFileLocation() const;
	FilePath makePathExpandedAndAbsolute(const FilePath& path) const;
	std::vector<FilePath> makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const;

	virtual std::vector<std::string> getAvailableLanguageStandards() const = 0;
	virtual SourceGroupType getType() const;

	std::string getStandard() const;
	void setStandard(const std::string& standard);

	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getSourcePathsExpandedAndAbsolute() const;
	void setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<FilePath> getExcludePaths() const;
	std::vector<FilePath> getExcludePathsExpandedAndAbsolute() const;
	void setExcludePaths(const std::vector<FilePath>& excludePaths);

	std::vector<std::string> getSourceExtensions() const;
	void setSourceExtensions(const std::vector<std::string>& sourceExtensions);

protected:
	const ProjectSettings* m_projectSettings;

private:
	virtual std::vector<std::string> getDefaultSourceExtensions() const = 0;
	virtual std::string getDefaultStandard() const = 0;

	std::string m_id;
	std::string m_name;
	const SourceGroupType m_type;

	std::string m_standard;
	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePath> m_excludePaths;
	std::vector<std::string> m_sourceExtensions;
};

#endif // SOURCE_GROUP_SETTINGS_H
