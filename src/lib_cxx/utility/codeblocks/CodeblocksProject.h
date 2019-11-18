#ifndef CODEBLOCKS_PROJECT_H
#define CODEBLOCKS_PROJECT_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "FilePath.h"

class ApplicationSettings;
class FilePath;
class IndexerCommandCxx;
class SourceGroupSettingsCxxCodeblocks;
class SourceGroupSettingsWithSourceExtensions;
class TextAccess;

namespace Codeblocks
{
class Target;
class Unit;

class Project
{
public:
	static std::shared_ptr<Project> load(const FilePath& projectFilePath);
	static std::shared_ptr<Project> load(std::shared_ptr<TextAccess> xmlAccess);

	std::set<FilePath> getAllSourceFilePathsCanonical(
		const std::vector<std::wstring>& sourceExtensions) const;
	std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const;

	std::vector<std::shared_ptr<IndexerCommandCxx>> getIndexerCommands(
		std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> appSettings) const;

private:
	Project(const FilePath& projectFilePath);

	FilePath m_projectFilePath;

	int m_versionMajor = 0;
	int m_versionMinor = 0;

	std::wstring m_title;

	std::vector<std::shared_ptr<Target>> m_targets;
	std::vector<std::shared_ptr<Unit>> m_units;
};
}	 // namespace Codeblocks

#endif	  // CODEBLOCKS_PROJECT_H
