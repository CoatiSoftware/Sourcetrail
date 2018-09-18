#ifndef SOURCE_GROUP_JAVA_GRADLE_H
#define SOURCE_GROUP_JAVA_GRADLE_H

#include <memory>
#include <vector>

#include "SourceGroupJava.h"

class SourceGroupSettingsJavaGradle;

class SourceGroupJavaGradle : public SourceGroupJava
{
public:
	SourceGroupJavaGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings);
	bool prepareIndexing() override;

private:
	std::vector<FilePath> getAllSourcePaths() const override;
	std::vector<FilePath> doGetClassPath() const override;
	std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() override;
	std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const override;
	bool prepareGradleData();

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
};

#endif // SOURCE_GROUP_JAVA_GRADLE_H
