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
	virtual SourceGroupType getType() const override;
	virtual bool prepareIndexing() override;

protected:
	virtual std::vector<FilePath> doGetClassPath() const override;

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() override;
	virtual std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const override;
	virtual std::vector<FilePath> getAllSourcePaths() const override;
	bool prepareGradleData();

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
};

#endif // SOURCE_GROUP_JAVA_GRADLE_H
