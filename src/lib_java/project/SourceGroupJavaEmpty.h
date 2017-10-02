#ifndef SOURCE_GROUP_JAVA_EMPTY_H
#define SOURCE_GROUP_JAVA_EMPTY_H

#include <memory>
#include <vector>

#include "project/SourceGroupJava.h"

class SourceGroupSettingsJavaEmpty;

class SourceGroupJavaEmpty: public SourceGroupJava
{
public:
	SourceGroupJavaEmpty(std::shared_ptr<SourceGroupSettingsJavaEmpty> settings);
	virtual ~SourceGroupJavaEmpty();

	virtual SourceGroupType getType() const;

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava();
	virtual std::vector<FilePath> getAllSourcePaths() const;

	std::shared_ptr<SourceGroupSettingsJavaEmpty> m_settings;
};

#endif // SOURCE_GROUP_JAVA_EMPTY_H
