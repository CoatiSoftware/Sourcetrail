#ifndef SOURCE_GROUP_SETTINGS_CXX_H
#define SOURCE_GROUP_SETTINGS_CXX_H

#include "settings/SourceGroupSettings.h"

class SourceGroupSettingsCxx
	: public SourceGroupSettings
{
public:
	SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	std::vector<std::string> getAvailableLanguageStandards() const override;

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpandedAndAbsolute() const;
	void setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpandedAndAbsolute() const;
	void setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	std::vector<std::wstring> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::wstring>& compilerFlags);

private:
	std::string getDefaultStandard() const override;

	std::vector<FilePath> m_headerSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	std::vector<std::wstring> m_compilerFlags;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_H
