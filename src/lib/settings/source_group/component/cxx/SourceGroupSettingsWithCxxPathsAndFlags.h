#ifndef SOURCE_GROUP_SETTINGS_WITH_CXX_PATHS_AND_FLAGS_H
#define SOURCE_GROUP_SETTINGS_WITH_CXX_PATHS_AND_FLAGS_H

#include <vector>

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithCxxPathsAndFlags
	: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithCxxPathsAndFlags() = default;

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpandedAndAbsolute() const;
	void setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpandedAndAbsolute() const;
	void setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	std::vector<std::wstring> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::wstring>& compilerFlags);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	std::vector<FilePath> m_headerSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	std::vector<std::wstring> m_compilerFlags;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CXX_PATHS_AND_FLAGS_H
