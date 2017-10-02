#ifndef SOURCE_GROUP_JAVA_GRADLE_H
#define SOURCE_GROUP_JAVA_GRADLE_H

#include <memory>
#include <vector>

#include "settings/SourceGroupSettingsJavaGradle.h"
#include "project/SourceGroupJava.h"

class SourceGroupJavaGradle : public SourceGroupJava
{
public:
	SourceGroupJavaGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings);
	virtual ~SourceGroupJavaGradle();
	virtual SourceGroupType getType() const;
	virtual bool prepareIndexing();

protected:
	virtual std::vector<FilePath> doGetClassPath();

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava();
	virtual std::vector<FilePath> getAllSourcePaths() const;
	bool prepareGradleData();

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
};

#endif // SOURCE_GROUP_JAVA_GRADLE_H
