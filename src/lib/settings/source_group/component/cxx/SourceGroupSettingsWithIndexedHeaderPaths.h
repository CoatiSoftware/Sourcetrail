#ifndef SOURCE_GROUP_SETTINGS_WITH_INDEXED_HEADER_PATHS_H
#define SOURCE_GROUP_SETTINGS_WITH_INDEXED_HEADER_PATHS_H

#include <vector>

#include "../SourceGroupSettingsComponent.h"

class FilePath;

class SourceGroupSettingsWithIndexedHeaderPaths: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithIndexedHeaderPaths() = default;

	std::vector<FilePath> getIndexedHeaderPaths() const;
	std::vector<FilePath> getIndexedHeaderPathsExpandedAndAbsolute() const;
	void setIndexedHeaderPaths(const std::vector<FilePath>& indexedHeaderPaths);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	std::vector<FilePath> m_indexedHeaderPaths;
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_INDEXED_HEADER_PATHS_H
