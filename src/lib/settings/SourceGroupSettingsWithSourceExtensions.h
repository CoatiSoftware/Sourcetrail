#ifndef SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_H
#define SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_H

#include <memory>
#include <string>
#include <vector>

#include "SourceGroupSettingsBase.h"

class ConfigManager;
class FilePath;

class SourceGroupSettingsWithSourceExtensions
	: virtual public SourceGroupSettingsBase
{
public:
	SourceGroupSettingsWithSourceExtensions();
	virtual ~SourceGroupSettingsWithSourceExtensions() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithSourceExtensions> other) const;

	std::vector<std::wstring> getSourceExtensions() const;
	void setSourceExtensions(const std::vector<std::wstring>& sourceExtensions);

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual std::vector<std::wstring> getDefaultSourceExtensions() const = 0;

	std::vector<std::wstring> m_sourceExtensions;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SOURCE_EXTENSIONS_H
