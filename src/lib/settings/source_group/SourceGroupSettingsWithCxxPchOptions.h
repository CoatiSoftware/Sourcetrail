#ifndef SOURCE_GROUP_SETTINGS_WITH_CXX_PCH_OPTIONS_H
#define SOURCE_GROUP_SETTINGS_WITH_CXX_PCH_OPTIONS_H

#include <memory>
#include <string>

#include "FilePath.h"
#include "SourceGroupSettingsBase.h"

class ConfigManager;

class SourceGroupSettingsWithCxxPchOptions
	: virtual public SourceGroupSettingsBase
{
public:
	SourceGroupSettingsWithCxxPchOptions() = default;
	virtual ~SourceGroupSettingsWithCxxPchOptions() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> other) const;

	FilePath getPchDependenciesDirectoryPath() const;

	FilePath getPchInputFilePath() const;
	FilePath getPchInputFilePathExpandedAndAbsolute() const;
	void setPchInputFilePathFilePath(const FilePath& path);

	std::vector<std::wstring> getPchFlags() const;
	void setPchFlags(const std::vector<std::wstring>& pchFlags);

	bool getUseCompilerFlags() const;
	void setUseCompilerFlags(bool useCompilerFlags);

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	FilePath m_pchInputFilePath;
	std::vector<std::wstring> m_pchFlags;
	bool m_useCompilerFlags = true;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CXX_PCH_OPTIONS_H
