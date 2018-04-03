#include "settings/SourceGroupSettings.h"

#include "utility/file/FileSystem.h"
#include "utility/utility.h"

const size_t SourceGroupSettings::s_version = 1;
const std::string SourceGroupSettings::s_keyPrefix = "source_groups/source_group_";

SourceGroupSettings::SourceGroupSettings(
	const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings
)
	: m_projectSettings(projectSettings)
	, m_id(id)
	, m_name(sourceGroupTypeToString(type))
	, m_type(type)
	, m_status(SOURCE_GROUP_STATUS_ENABLED)
	, m_standard("")
	, m_sourcePaths(std::vector<FilePath>())
	, m_excludeFilters(std::vector<std::wstring>())
	, m_sourceExtensions(std::vector<std::wstring>())
{
}

SourceGroupSettings::~SourceGroupSettings()
{
}

void SourceGroupSettings::load(std::shared_ptr<const ConfigManager> config)
{
	const std::string key = s_keyPrefix + getId();

	const std::string name = getValue<std::string>(key + "/name", "", config);
	if (!name.empty())
	{
		setName(name);
	}

	setStatus(stringToSourceGroupStatusType(
		getValue(key + "/status", sourceGroupStatusTypeToString(SOURCE_GROUP_STATUS_ENABLED), config)));
	setStandard(getValue<std::string>(key + "/standard", "", config));
	setSourcePaths(getPathValues(key + "/source_paths/source_path", config));
	setExcludeFilterStrings(getValues(key + "/exclude_filters/exclude_filter", std::vector<std::wstring>(), config));
	setSourceExtensions(getValues(key + "/source_extensions/source_extension", std::vector<std::wstring>(), config));
}

void SourceGroupSettings::save(std::shared_ptr<ConfigManager> config)
{
	const std::string key = s_keyPrefix + getId();

	setValue(key + "/status", sourceGroupStatusTypeToString(getStatus()), config);
	setValue(key + "/name", getName(), config);
	setValue(key + "/standard", getStandard(), config);
	setPathValues(key + "/source_paths/source_path", getSourcePaths(), config);
	setValues(key + "/exclude_filters/exclude_filter", getExcludeFilterStrings(), config);
	setValues(key + "/source_extensions/source_extension", getSourceExtensions(), config);
}

bool SourceGroupSettings::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	return (
		m_id == other->m_id &&
		m_name == other->m_name &&
		m_type == other->m_type &&
		m_status == other->m_status &&
		m_standard == other->m_standard &&
		utility::isPermutation(m_sourcePaths, other->m_sourcePaths) &&
		utility::isPermutation(m_excludeFilters, other->m_excludeFilters) &&
		utility::isPermutation(m_sourceExtensions, other->m_sourceExtensions)
	);
}

std::string SourceGroupSettings::getId() const
{
	return m_id;
}

void SourceGroupSettings::setId(const std::string& id)
{
	m_id = id;
}

std::string SourceGroupSettings::getName() const
{
	return m_name;
}

void SourceGroupSettings::setName(const std::string& name)
{
	m_name = name;
}

FilePath SourceGroupSettings::getProjectDirectoryPath() const
{
	return m_projectSettings->getProjectDirectoryPath();
}

FilePath SourceGroupSettings::makePathExpandedAndAbsolute(const FilePath& path) const
{
	return m_projectSettings->makePathExpandedAndAbsolute(path);
}

std::vector<FilePath> SourceGroupSettings::makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(paths);
}

SourceGroupType SourceGroupSettings::getType() const
{
	return m_type;
}

SourceGroupStatusType SourceGroupSettings::getStatus() const
{
	return m_status;
}

void SourceGroupSettings::setStatus(SourceGroupStatusType status)
{
	m_status = status;
}

std::string SourceGroupSettings::getStandard() const
{
	if (m_standard.empty())
	{
		return getDefaultStandard();
	}
	return m_standard;
}

void SourceGroupSettings::setStandard(const std::string& standard)
{
	m_standard = standard;
}

std::vector<FilePath> SourceGroupSettings::getSourcePaths() const
{
	return m_sourcePaths;
}

std::vector<FilePath> SourceGroupSettings::getSourcePathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getSourcePaths());
}

void SourceGroupSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	m_sourcePaths = sourcePaths;
}

std::vector<std::wstring> SourceGroupSettings::getExcludeFilterStrings() const
{
	return m_excludeFilters;
}

std::vector<FilePathFilter> SourceGroupSettings::getExcludeFiltersExpandedAndAbsolute() const
{
	std::vector<FilePathFilter> result;

	for (const FilePathFilter& filter : m_excludeFilters)
	{
		const std::wstring filterString = filter.wstr();
		const size_t wildcardPos = filterString.find(L"*");
		if (wildcardPos != filterString.npos)
		{
			std::wsmatch match;
			if (std::regex_search(filterString, match, std::wregex(L"[\\\\/]")) && !match.empty() &&
				match.position(0) < int(wildcardPos))
			{
				const FilePath p = m_projectSettings->makePathExpandedAndAbsolute(FilePath(match.prefix().str()));
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
				result.push_back(filter);
			}
		}
		else
		{
			const FilePath p = m_projectSettings->makePathExpandedAndAbsolute(FilePath(filterString));
			const bool isFile = p.exists() && !p.isDirectory();

			std::set<FilePath> symLinkPaths = FileSystem::getSymLinkedDirectories(p);
			symLinkPaths.insert(p);

			utility::append(result,
				utility::convert<FilePath, FilePathFilter>(
					utility::toVector(symLinkPaths),
					[isFile](const FilePath& filePath)
					{
						return FilePathFilter(filePath.wstr() + (isFile ? L"" : L"**"));
					}
				)
			);
		}
	}

	return result;
}

void SourceGroupSettings::setExcludeFilterStrings(const std::vector<std::wstring>& excludeFilters)
{
	m_excludeFilters = excludeFilters;
}

std::vector<std::wstring> SourceGroupSettings::getSourceExtensions() const
{
	if (m_sourceExtensions.empty())
	{
		return getDefaultSourceExtensions();
	}
	return m_sourceExtensions;
}

void SourceGroupSettings::setSourceExtensions(const std::vector<std::wstring>& sourceExtensions)
{
	m_sourceExtensions = sourceExtensions;
}

std::vector<FilePath> SourceGroupSettings::getPathValues(
	const std::string& key, std::shared_ptr<const ConfigManager> config)
{
	std::vector<FilePath> paths;
	for (const std::wstring& value : getValues<std::wstring>(key, {}, config))
	{
		paths.push_back(FilePath(value));
	}
	return paths;
}

bool SourceGroupSettings::setPathValues(
	const std::string& key, const std::vector<FilePath>& paths, std::shared_ptr<ConfigManager> config)
{
	std::vector<std::wstring> values;
	for (const FilePath& path : paths)
	{
		values.push_back(path.wstr());
	}

	return setValues(key, values, config);
}
