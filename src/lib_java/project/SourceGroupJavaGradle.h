#ifndef SOURCE_GROUP_JAVA_GRADLE_H
#define SOURCE_GROUP_JAVA_GRADLE_H

#include <memory>
#include <vector>

#include "../../lib/utility/SingleValueCache.h"
#include "SourceGroupJava.h"

class SourceGroupSettingsJavaGradle;

class SourceGroupJavaGradle: public SourceGroupJava
{
public:
	SourceGroupJavaGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings);
	bool prepareIndexing() override;

private:
	std::vector<FilePath> getAllSourcePaths() const override;
	std::vector<FilePath> doGetClassPath() const override;
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;
	bool prepareGradleData();
	std::vector<FilePath> doGetAllSourcePaths() const;

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
	mutable SingleValueCache<std::vector<FilePath>> m_allSourcePathsCache;
};

#endif	  // SOURCE_GROUP_JAVA_GRADLE_H
