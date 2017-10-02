#include "settings/SourceGroupSettingsCxx.h"

#include "utility/utility.h"
#include "utility/utilityApp.h"

SourceGroupSettingsCxx::SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_headerSearchPaths(std::vector<FilePath>())
	, m_frameworkSearchPaths(std::vector<FilePath>())
	, m_compilerFlags(std::vector<std::string>())
	, m_shouldApplyAnonymousTypedefTransformation(true)
{
}

SourceGroupSettingsCxx::~SourceGroupSettingsCxx()
{
}

void SourceGroupSettingsCxx::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	setHeaderSearchPaths(getPathValues(key + "/header_search_paths/header_search_path", config));
	setFrameworkSearchPaths(getPathValues(key + "/framework_search_paths/framework_search_path", config));
	setCompilerFlags(getValues<std::string>(key + "/compiler_flags/compiler_flag", std::vector<std::string>(), config));
	setShouldApplyAnonymousTypedefTransformation(getValue<bool>(key + "/should_apply_anonymous_typedef_transformation", true, config));
}

void SourceGroupSettingsCxx::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	setPathValues(key + "/header_search_paths/header_search_path", getHeaderSearchPaths(), config);
	setPathValues(key + "/framework_search_paths/framework_search_path", getFrameworkSearchPaths(), config);
	setValues(key + "/compiler_flags/compiler_flag", getCompilerFlags(), config);
	setValue(key + "/should_apply_anonymous_typedef_transformation", getShouldApplyAnonymousTypedefTransformation(), config);
}

bool SourceGroupSettingsCxx::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxx> otherCxx = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(other);

	return (
		otherCxx &&
		SourceGroupSettings::equals(other) &&
		utility::isPermutation(m_headerSearchPaths, otherCxx->m_headerSearchPaths) &&
		utility::isPermutation(m_frameworkSearchPaths, otherCxx->m_frameworkSearchPaths) &&
		utility::isPermutation(m_compilerFlags, otherCxx->m_compilerFlags)
	);
}

std::vector<std::string> SourceGroupSettingsCxx::getAvailableLanguageStandards() const
{
	std::vector<std::string> standards;

	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
	case SOURCE_GROUP_CXX_CDB:
		standards.push_back("c++1z");
		standards.push_back("gnu++1z");

		standards.push_back("c++14");
		standards.push_back("gnu++14");

		standards.push_back("c++1y");
		standards.push_back("gnu++1y");

		standards.push_back("c++11");
		standards.push_back("gnu++11");

		standards.push_back("c++0x");
		standards.push_back("gnu++0x");

		standards.push_back("c++03");

		standards.push_back("c++98");
		standards.push_back("gnu++98");
		break;

	case SOURCE_GROUP_C_EMPTY:
		standards.push_back("c1x");
		standards.push_back("gnu1x");
		standards.push_back("iso9899:201x");

		standards.push_back("c11");
		standards.push_back("gnu11");
		standards.push_back("iso9899:2011");

		standards.push_back("c9x");
		standards.push_back("gnu9x");
		standards.push_back("iso9899:199x");

		standards.push_back("c99");
		standards.push_back("gnu99");
		standards.push_back("iso9899:1999");

		standards.push_back("iso9899:199409");

		standards.push_back("c90");
		standards.push_back("gnu90");
		standards.push_back("iso9899:1990");

		standards.push_back("c89");
		standards.push_back("gnu89");
		break;

	default:
		break;
	}

	return standards;
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPaths() const
{
	return m_headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getHeaderSearchPaths());
}

void SourceGroupSettingsCxx::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	m_headerSearchPaths = headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getFrameworkSearchPaths());
}

void SourceGroupSettingsCxx::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	m_frameworkSearchPaths = frameworkSearchPaths;
}

std::vector<std::string> SourceGroupSettingsCxx::getCompilerFlags() const
{
	return m_compilerFlags;
}

void SourceGroupSettingsCxx::setCompilerFlags(const std::vector<std::string>& compilerFlags)
{
	m_compilerFlags = compilerFlags;
}

bool SourceGroupSettingsCxx::getShouldApplyAnonymousTypedefTransformation() const
{
	return m_shouldApplyAnonymousTypedefTransformation;
}

void SourceGroupSettingsCxx::setShouldApplyAnonymousTypedefTransformation(bool shouldApplyAnonymousTypedefTransformation)
{
	m_shouldApplyAnonymousTypedefTransformation = shouldApplyAnonymousTypedefTransformation;
}

std::vector<std::string> SourceGroupSettingsCxx::getDefaultSourceExtensions() const
{
	std::vector<std::string> defaultValues;

	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
		defaultValues.push_back(".cpp");
		defaultValues.push_back(".cxx");
		defaultValues.push_back(".cc");
		break;
	case SOURCE_GROUP_C_EMPTY:
		defaultValues.push_back(".c");
		break;
	case SOURCE_GROUP_CXX_CDB:
	default:
		break;
	}

	return defaultValues;
}

std::string SourceGroupSettingsCxx::getDefaultStandard() const
{
	switch (getType())
	{
	case SOURCE_GROUP_CPP_EMPTY:
		return "c++1z";
	case SOURCE_GROUP_C_EMPTY:
		return "c1x";
	case SOURCE_GROUP_CXX_CDB:
	default:
		break;
	}

	return "";
}
