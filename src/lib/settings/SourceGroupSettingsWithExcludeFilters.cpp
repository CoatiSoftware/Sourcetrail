#include "SourceGroupSettingsWithExcludeFilters.h"

#include "ProjectSettings.h"
#include "FilePathFilter.h"
#include "FileSystem.h"
#include "utility.h"

SourceGroupSettingsWithExcludeFilters::SourceGroupSettingsWithExcludeFilters()
	: m_excludeFilters(std::vector<std::wstring>())
{
}

bool SourceGroupSettingsWithExcludeFilters::equals(std::shared_ptr<SourceGroupSettingsWithExcludeFilters> other) const
{
	return (
		utility::isPermutation(m_excludeFilters, other->m_excludeFilters)
	);
}

void SourceGroupSettingsWithExcludeFilters::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setExcludeFilterStrings(config->getValuesOrDefaults(key + "/exclude_filters/exclude_filter", std::vector<std::wstring>()));
}

void SourceGroupSettingsWithExcludeFilters::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/exclude_filters/exclude_filter", getExcludeFilterStrings());
}


std::vector<std::wstring> SourceGroupSettingsWithExcludeFilters::getExcludeFilterStrings() const
{
	return m_excludeFilters;
}

std::vector<FilePathFilter> SourceGroupSettingsWithExcludeFilters::getExcludeFiltersExpandedAndAbsolute() const
{
	return getFiltersExpandedAndAbsolute(getExcludeFilterStrings());
}

void SourceGroupSettingsWithExcludeFilters::setExcludeFilterStrings(const std::vector<std::wstring>& excludeFilters)
{
	m_excludeFilters = excludeFilters;
}

std::vector<FilePathFilter> SourceGroupSettingsWithExcludeFilters::getFiltersExpandedAndAbsolute(const std::vector<std::wstring>& filterStrings) const
{
	std::vector<FilePathFilter> result;

	for (const std::wstring& filterString : filterStrings)
	{
		if (!filterString.empty())
		{
			const size_t wildcardPos = filterString.find(L"*");
			if (wildcardPos != filterString.npos)
			{
				std::wsmatch match;
				if (std::regex_search(filterString, match, std::wregex(L"[\\\\/]")) && !match.empty() &&
					match.position(0) < int(wildcardPos))
				{
					const FilePath p = getProjectSettings()->makePathExpandedAndAbsolute(FilePath(match.prefix().str()));
					std::set<FilePath> symLinkPaths = FileSystem::getSymLinkedDirectories(p);
					symLinkPaths.insert(p);

					utility::append(result,
						utility::convert<FilePath, FilePathFilter>(
							utility::toVector(symLinkPaths),
							[match](const FilePath& filePath)
							{
								return FilePathFilter(filePath.wstr() + L"/" + match.suffix().str());
							}
						)
					);
				}
				else
				{
					result.push_back(FilePathFilter(filterString));
				}
			}
			else
			{
				const FilePath p = getProjectSettings()->makePathExpandedAndAbsolute(FilePath(filterString));
				const bool isFile = p.exists() && !p.isDirectory();

				std::set<FilePath> symLinkPaths = FileSystem::getSymLinkedDirectories(p);
				symLinkPaths.insert(p);

				utility::append(result,
					utility::convert<FilePath, FilePathFilter>
					(
						utility::toVector(symLinkPaths),
						[isFile](const FilePath& filePath)
						{
							return FilePathFilter(filePath.wstr() + (isFile ? L"" : L"**"));
						}
					)
				);
			}
		}
	}

	return result;
}
