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
	virtual SourceGroupType getType() const;
	virtual bool prepareIndexing();

protected:
	virtual std::vector<FilePath> doGetClassPath();

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava();
	virtual std::vector<FilePath> getAllSourcePaths() const;
	bool prepareMavenData();

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
};

#endif // SOURCE_GROUP_JAVA_MAVEN_H
