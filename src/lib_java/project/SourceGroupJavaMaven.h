#ifndef SOURCE_GROUP_JAVA_MAVEN_H
#define SOURCE_GROUP_JAVA_MAVEN_H

#include <memory>
#include <vector>

#include "settings/SourceGroupSettingsJavaMaven.h"
#include "project/SourceGroupJava.h"

class SourceGroupJavaMaven: public SourceGroupJava
{
public:
	SourceGroupJavaMaven(std::shared_ptr<SourceGroupSettingsJavaMaven> settings);
	virtual ~SourceGroupJavaMaven();
	virtual SourceGroupType getType() const override;
	virtual bool prepareIndexing() override;

protected:
	virtual std::vector<FilePath> doGetClassPath() const override;

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() override;
	virtual std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const override;
	virtual std::vector<FilePath> getAllSourcePaths() const override;
	bool prepareMavenData();

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
};

#endif // SOURCE_GROUP_JAVA_MAVEN_H
