#include "settings/SourceGroupSettingsCxx.h"

#include "utility/utility.h"

SourceGroupSettingsCxx::SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_headerSearchPaths(std::vector<FilePath>())
	, m_frameworkSearchPaths(std::vector<FilePath>())
	, m_compilerFlags(std::vector<std::string>())
	, m_useSourcePathsForHeaderSearch(false)
	, m_hasDefinedUseSourcePathsForHeaderSearch(false)
	, m_compilationDatabasePath(FilePath())
{
}

SourceGroupSettingsCxx::~SourceGroupSettingsCxx()
{
}

bool SourceGroupSettingsCxx::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxx> otherCxx = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(other);

	return (
		otherCxx &&
		SourceGroupSettings::equals(other) &&
		utility::isPermutation(m_headerSearchPaths, otherCxx->m_headerSearchPaths) &&
		utility::isPermutation(m_frameworkSearchPaths, otherCxx->m_frameworkSearchPaths) &&
		utility::isPermutation(m_compilerFlags, otherCxx->m_compilerFlags) &&
		m_useSourcePathsForHeaderSearch == otherCxx->m_useSourcePathsForHeaderSearch &&
		m_hasDefinedUseSourcePathsForHeaderSearch == otherCxx->m_hasDefinedUseSourcePathsForHeaderSearch &&
		m_compilationDatabasePath == otherCxx->m_compilationDatabasePath
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

std::vector<FilePath> SourceGroupSettingsCxx::getAbsoluteHeaderSearchPaths() const
{
	return m_projectSettings->makePathsAbsolute(getHeaderSearchPaths());
}

void SourceGroupSettingsCxx::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	m_headerSearchPaths = headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getAbsoluteFrameworkSearchPaths() const
{
	return m_projectSettings->makePathsAbsolute(getFrameworkSearchPaths());
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

bool SourceGroupSettingsCxx::getUseSourcePathsForHeaderSearch() const
{
	return m_useSourcePathsForHeaderSearch;
}

void SourceGroupSettingsCxx::setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch)
{
	m_useSourcePathsForHeaderSearch = useSourcePathsForHeaderSearch;
}

bool SourceGroupSettingsCxx::getHasDefinedUseSourcePathsForHeaderSearch() const
{
	return m_hasDefinedUseSourcePathsForHeaderSearch;
}

void SourceGroupSettingsCxx::setHasDefinedUseSourcePathsForHeaderSearch(bool hasDefinedUseSourcePathsForHeaderSearch)
{
	m_hasDefinedUseSourcePathsForHeaderSearch = hasDefinedUseSourcePathsForHeaderSearch;
}

FilePath SourceGroupSettingsCxx::getCompilationDatabasePath() const
{
	return m_compilationDatabasePath;
}

FilePath SourceGroupSettingsCxx::getAbsoluteCompilationDatabasePath() const
{
	return m_projectSettings->makePathAbsolute(getCompilationDatabasePath());
}

void SourceGroupSettingsCxx::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	m_compilationDatabasePath = compilationDatabasePath;
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
