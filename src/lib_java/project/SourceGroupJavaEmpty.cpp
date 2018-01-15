#include "project/SourceGroupJavaEmpty.h"

#include "settings/SourceGroupSettingsJavaEmpty.h"

SourceGroupJavaEmpty::SourceGroupJavaEmpty(std::shared_ptr<SourceGroupSettingsJavaEmpty> settings)
	: m_settings(settings)
{
}

SourceGroupJavaEmpty::~SourceGroupJavaEmpty()
{
}

SourceGroupType SourceGroupJavaEmpty::getType() const
{
	return SOURCE_GROUP_JAVA_EMPTY;
}

std::shared_ptr<SourceGroupSettingsJava> SourceGroupJavaEmpty::getSourceGroupSettingsJava()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettingsJava> SourceGroupJavaEmpty::getSourceGroupSettingsJava() const
{
	return m_settings;
}

std::vector<FilePath> SourceGroupJavaEmpty::getAllSourcePaths() const
{
	return m_settings->getSourcePathsExpandedAndAbsolute();
}
