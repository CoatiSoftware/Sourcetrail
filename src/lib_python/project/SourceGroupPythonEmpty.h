#ifndef SOURCE_GROUP_PYTHON_EMPTY_H
#define SOURCE_GROUP_PYTHON_EMPTY_H

#include <memory>
#include <vector>

#include "SourceGroup.h"

class SourceGroupSettingsPythonEmpty;

class SourceGroupPythonEmpty: public SourceGroup
{
public:
	SourceGroupPythonEmpty(std::shared_ptr<SourceGroupSettingsPythonEmpty> settings);

	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsPythonEmpty> m_settings;
};

#endif // SOURCE_GROUP_PYTHON_EMPTY_H
