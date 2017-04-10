#include "settings/SourceGroupSettings.h"

#include "utility/utility.h"

SourceGroupSettings::SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: m_projectSettings(projectSettings)
	, m_id(id)
	, m_type(type)
{
}

SourceGroupSettings::~SourceGroupSettings()
{
}

bool SourceGroupSettings::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	return (
		m_id == other->m_id &&
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

FilePath SourceGroupSettings::getProjectFileLocation() const
{
	return m_projectSettings->getProjectFileLocation();
}

FilePath SourceGroupSettings::makePathAbsolute(const FilePath& path) const
{
	return m_projectSettings->makePathAbsolute(path);
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

std::vector<FilePath> SourceGroupSettings::getAbsoluteSourcePaths() const
{
	return m_projectSettings->makePathsAbsolute(getSourcePaths());
}

void SourceGroupSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	m_sourcePaths = sourcePaths;
}

std::vector<FilePath> SourceGroupSettings::getExcludePaths() const
{
	return m_excludePaths;
}

std::vector<FilePath> SourceGroupSettings::getAbsoluteExcludePaths() const
{
	return m_projectSettings->makePathsAbsolute(getExcludePaths());
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
