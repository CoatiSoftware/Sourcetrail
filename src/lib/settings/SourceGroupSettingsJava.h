#ifndef SOURCE_GROUP_SETTINGS_JAVA_H
#define SOURCE_GROUP_SETTINGS_JAVA_H

#include <memory>
#include <vector>

#include "settings/SourceGroupSettings.h"

class SourceGroupSettingsJava
	: public SourceGroupSettings
{
public:
	SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsJava();

	virtual void load(std::shared_ptr<const ConfigManager> config) override;
	virtual void save(std::shared_ptr<ConfigManager> config) override;

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	virtual std::vector<std::string> getAvailableLanguageStandards() const override;

	bool getUseJreSystemLibrary() const;
	void setUseJreSystemLibrary(bool useJreSystemLibrary);

	std::vector<FilePath> getClasspath() const;
	std::vector<FilePath> getClasspathExpandedAndAbsolute() const;
	void setClasspath(const std::vector<FilePath>& classpath);

private:
	virtual std::vector<std::wstring> getDefaultSourceExtensions() const override;
	virtual std::string getDefaultStandard() const override;

	bool m_useJreSystemLibrary;
	std::vector<FilePath> m_classpath;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_H
