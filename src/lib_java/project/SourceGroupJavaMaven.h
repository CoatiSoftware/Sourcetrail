#ifndef SOURCE_GROUP_JAVA_MAVEN_H
#define SOURCE_GROUP_JAVA_MAVEN_H

#include <memory>
#include <vector>

#include "SingleValueCache.h"
#include "SourceGroupJava.h"

class SourceGroupSettingsJavaMaven;

class SourceGroupJavaMaven: public SourceGroupJava
{
public:
	SourceGroupJavaMaven(std::shared_ptr<SourceGroupSettingsJavaMaven> settings);
	bool prepareIndexing() override;

private:
	std::vector<FilePath> getAllSourcePaths() const override;
	std::vector<FilePath> doGetClassPath() const override;
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;
	bool prepareMavenData();
	std::vector<FilePath> doGetAllSourcePaths() const;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
	mutable SingleValueCache<std::vector<FilePath>> m_allSourcePathsCache;
};

#endif // SOURCE_GROUP_JAVA_MAVEN_H
