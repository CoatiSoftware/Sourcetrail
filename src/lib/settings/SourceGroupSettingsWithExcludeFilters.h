#ifndef SOURCE_GROUP_SETTINGS_WITH_EXCLUDE_FILTERS_H
#define SOURCE_GROUP_SETTINGS_WITH_EXCLUDE_FILTERS_H

#include <memory>
#include <string>
#include <vector>

#include "SourceGroupSettingsBase.h"

class ConfigManager;
class FilePathFilter;

class SourceGroupSettingsWithExcludeFilters
	: virtual public SourceGroupSettingsBase
{
public:
	SourceGroupSettingsWithExcludeFilters();
	virtual ~SourceGroupSettingsWithExcludeFilters() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithExcludeFilters> other) const;

	std::vector<std::wstring> getExcludeFilterStrings() const;
	std::vector<FilePathFilter> getExcludeFiltersExpandedAndAbsolute() const;
	void setExcludeFilterStrings(const std::vector<std::wstring>& excludeFilters);

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	std::vector<FilePathFilter> getFiltersExpandedAndAbsolute(const std::vector<std::wstring>& filterStrings) const;

	std::vector<std::wstring> m_excludeFilters;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_EXCLUDE_FILTERS_H
