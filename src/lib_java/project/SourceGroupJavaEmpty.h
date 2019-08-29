#ifndef SOURCE_GROUP_JAVA_EMPTY_H
#define SOURCE_GROUP_JAVA_EMPTY_H

#include <memory>
#include <vector>

#include "SourceGroupJava.h"

class SourceGroupSettingsJavaEmpty;

class SourceGroupJavaEmpty
	: public SourceGroupJava
{
public:
	SourceGroupJavaEmpty(std::shared_ptr<SourceGroupSettingsJavaEmpty> settings);
	bool prepareIndexing() override;

private:
	std::vector<FilePath> getAllSourcePaths() const override;
	std::vector<FilePath> doGetClassPath() const override;
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsJavaEmpty> m_settings;
};

#endif // SOURCE_GROUP_JAVA_EMPTY_H
