#ifndef CODEBLOCKS_PROJECT_H
#define CODEBLOCKS_PROJECT_H

#include <memory>
#include <set>
#include <string>
#include <vector>

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
			const std::vector<std::wstring>& sourceExtensions
		) const;
		std::set<FilePath> getAllSourceFilePaths(
			const std::vector<std::wstring>& sourceExtensions
		) const;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const;

		std::vector<std::shared_ptr<IndexerCommandCxx>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const;

	private:
		Project() = default;

		int m_versionMajor;
		int m_versionMinor;

		std::wstring m_title;

		std::vector<std::shared_ptr<Target>> m_targets;
		std::vector<std::shared_ptr<Unit>> m_units;
	};
}

#endif // CODEBLOCKS_PROJECT_H
