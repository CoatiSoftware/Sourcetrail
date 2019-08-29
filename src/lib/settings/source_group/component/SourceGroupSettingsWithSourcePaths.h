#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H

#include <vector>

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithSourcePaths
	: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithSourcePaths() = default;

	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getSourcePathsExpandedAndAbsolute() const;
	void setSourcePaths(const std::vector<FilePath>& sourcePaths);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	std::vector<FilePath> m_sourcePaths;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_PATHS_H
