#ifndef SOURCE_GROUP_JAVA_SONARGRAPH_H
#define SOURCE_GROUP_JAVA_SONARGRAPH_H

#include <memory>
#include <set>

#include "SourceGroup.h"

class SourceGroupSettingsJavaSonargraph;

class SourceGroupJavaSonargraph
	: public SourceGroup
{
public:
	SourceGroupJavaSonargraph(std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings);

	bool prepareIndexing() override;
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const RefreshInfo& info) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsJavaSonargraph> m_settings;
};

#endif // SOURCE_GROUP_JAVA_SONARGRAPH_H
