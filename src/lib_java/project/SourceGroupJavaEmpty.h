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

	virtual SourceGroupType getType() const override;

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() override;
	virtual std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const override;
	virtual std::vector<FilePath> getAllSourcePaths() const override;

	std::shared_ptr<SourceGroupSettingsJavaEmpty> m_settings;
};

#endif // SOURCE_GROUP_JAVA_EMPTY_H
