#include "settings/SourceGroupSettings.h"

#include "utility/utility.h"

SourceGroupSettings::SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: m_projectSettings(projectSettings)
	, m_id(id)
	, m_name(sourceGroupTypeToString(type))
	, m_type(type)
	, m_standard("")
	, m_sourcePaths(std::vector<FilePath>())
	, m_excludePaths(std::vector<FilePath>())
	, m_sourceExtensions(std::vector<std::string>())
{
}

SourceGroupSettings::~SourceGroupSettings()
{
}

bool SourceGroupSettings::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	return (
		m_id == other->m_id &&
		m_name == other->m_name &&
		m_type == other->m_type &&
		m_standard == other->m_standard &&
		utility::isPermutation(m_sourcePaths, other->m_sourcePaths) &&
		utility::isPermutation(m_excludePaths, other->m_excludePaths) &&
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

FilePath SourceGroupSettings::getProjectFileLocation() const
{
	return m_projectSettings->getProjectFileLocation();
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

std::vector<FilePath> SourceGroupSettings::getExcludePaths() const
{
	return m_excludePaths;
}

std::vector<FilePath> SourceGroupSettings::getExcludePathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getExcludePaths());
}

void SourceGroupSettings::setExcludePaths(const std::vector<FilePath>& excludePaths)
{
	m_excludePaths = excludePaths;
}

std::vector<std::string> SourceGroupSettings::getSourceExtensions() const
{
	if (m_sourceExtensions.empty())
	{
		return getDefaultSourceExtensions();
	}
	return m_sourceExtensions;
}

void SourceGroupSettings::setSourceExtensions(const std::vector<std::string>& sourceExtensions)
{
	m_sourceExtensions = sourceExtensions;
}
