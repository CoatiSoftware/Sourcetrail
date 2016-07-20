#include "settings/ApplicationSettings.h"

#include "utility/ResourcePaths.h"
#include "utility/utility.h"

std::shared_ptr<ApplicationSettings> ApplicationSettings::s_instance;

std::shared_ptr<ApplicationSettings> ApplicationSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ApplicationSettings>(new ApplicationSettings());
	}

	return s_instance;
}

ApplicationSettings::ApplicationSettings()
{
}

ApplicationSettings::~ApplicationSettings()
{
}

bool ApplicationSettings::operator==(const ApplicationSettings& other) const
{
	return
		utility::isPermutation<FilePath>(getHeaderSearchPaths(), other.getHeaderSearchPaths()) &&
		utility::isPermutation<FilePath>(getFrameworkSearchPaths(), other.getFrameworkSearchPaths());
}

int ApplicationSettings::getMaxRecentProjectsCount() const
{
	return 7;
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPaths() const
{
	return getPathValues("source/header_search_paths/header_search_path");
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPathsExpanded() const
{
	std::vector<FilePath> paths = getPathValues("source/header_search_paths/header_search_path");
	expandPaths(paths);
	return paths;
}

bool ApplicationSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPaths() const
{
	return getPathValues("source/framework_search_paths/framework_search_path");
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPathsExpanded() const
{
	std::vector<FilePath> paths = getPathValues("source/framework_search_paths/framework_search_path");
	expandPaths(paths);
	return paths;
}

bool ApplicationSettings::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	return setPathValues("source/framework_search_paths/framework_search_path", frameworkSearchPaths);
}

std::vector<std::string> ApplicationSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/compiler_flags/compiler_flag", defaultValues);
}

std::string ApplicationSettings::getFontName() const
{
	return getValue<std::string>("application/font_name", "Source Code Pro");
}

void ApplicationSettings::setFontName(const std::string& fontName)
{
	setValue<std::string>("application/font_name", fontName);
}

int ApplicationSettings::getFontSize() const
{
	return getValue<int>("application/font_size", 14);
}

void ApplicationSettings::setFontSize(int fontSize)
{
	setValue<int>("application/font_size", fontSize);
}

FilePath ApplicationSettings::getColorSchemePath() const
{
	FilePath defaultPath(ResourcePaths::getColorSchemesPath() + "bright.xml");
	FilePath path(getValue<std::string>("application/color_scheme", defaultPath.str()));

	if (path != defaultPath && !path.exists())
	{
		return defaultPath;
	}

	return path;
}

void ApplicationSettings::setColorSchemePath(const FilePath& colorSchemePath)
{
	setValue<std::string>("application/color_scheme", colorSchemePath.str());
}

int ApplicationSettings::getFontSizeMax() const
{
	return getValue<int>("application/font_size_max", 24);
}

void ApplicationSettings::setFontSizeMax(const int fontSizeMax)
{
	setValue<int>("application/font_size_max", fontSizeMax);
}

int ApplicationSettings::getFontSizeMin() const
{
	return getValue<int>("application/font_size_min", 4);
}

void ApplicationSettings::setFontSizeMin(const int fontSizeMin)
{
	setValue<int>("application/font_size_min", fontSizeMin);
}

int ApplicationSettings::getFontSizeStd() const
{
	return getValue<int>("application/font_size_std", 12);
}

void ApplicationSettings::setFontSizeStd(const int fontSizeStd)
{
	setValue<int>("application/font_size_std", fontSizeStd);
}

int ApplicationSettings::getIndexerThreadCount() const
{
	return getValue<int>("application/indexer_thread_count", 4);
}

void ApplicationSettings::setIndexerThreadCount(const int count)
{
	setValue<int>("application/indexer_thread_count", count);
}

bool ApplicationSettings::getShowExternalNonFatalErrors() const
{
	return getValue<bool>("application/show_external_non_fatal_errors", false);
}

void ApplicationSettings::setShowExternalNonFatalErrors(const bool show)
{
	setValue<bool>("application/show_external_non_fatal_errors", show);
}

int ApplicationSettings::getWindowBaseWidth() const
{
	return getValue<int>("application/window_base_width", 500);
}

int ApplicationSettings::getWindowBaseHeight() const
{
	return getValue<int>("application/window_base_height", 500);
}

int ApplicationSettings::getCodeTabWidth() const
{
	return getValue<int>("code/tab_width", 4);
}

void ApplicationSettings::setCodeTabWidth(int codeTabWidth)
{
	setValue<int>("code/tab_width", codeTabWidth);
}

int ApplicationSettings::getCodeSnippetSnapRange() const
{
	return getValue<int>("code/snippet/snap_range", 4);
}

void ApplicationSettings::setCodeSnippetSnapRange(int range)
{
	setValue<int>("code/snippet/snap_range", range);
}

int ApplicationSettings::getCodeSnippetExpandRange() const
{
	return getValue<int>("code/snippet/expand_range", 2);
}

void ApplicationSettings::setCodeSnippetExpandRange(int range)
{
	setValue<int>("code/snippet/expand_range", range);
}

std::vector<FilePath> ApplicationSettings::getRecentProjects() const
{
	std::vector<FilePath> recentProjects;
	std::vector<FilePath> loadedRecentProjects = getPathValues("user/recent_projects/recent_project");

	for (FilePath project: loadedRecentProjects)
	{
			recentProjects.push_back(project);
	}
	return recentProjects;
}

bool ApplicationSettings::setRecentProjects(const std::vector<FilePath> &recentProjects)
{
	return setPathValues("user/recent_projects/recent_project", recentProjects);
}

int ApplicationSettings::getPluginPort() const
{
	return getValue<int>("network/plugin_port", 6666);
}

void ApplicationSettings::setPluginPort(const int pluginPort)
{
	setValue<int>("network/plugin_port", pluginPort);
}

int ApplicationSettings::getCoatiPort() const
{
	return getValue<int>("network/coati_port", 6667);
}

void ApplicationSettings::setCoatiPort(const int coatiPort)
{
	setValue<int>("network/coati_port", coatiPort);
}

int ApplicationSettings::getControlsMouseBackButton() const
{
	return getValue<int>("controls/mouse_back_button", 0x8);
}

int ApplicationSettings::getControlsMouseForwardButton() const
{
	return getValue<int>("controls/mouse_forward_button", 0x10);
}

std::string ApplicationSettings::getLicenseString() const
{
	return getValue<std::string>("user/license/license", "");
}

std::string ApplicationSettings::getLicenseCheck() const
{
	return getValue<std::string>("user/license/check", "");
}

void ApplicationSettings::setLicenseString(const std::string& licenseString)
{
	setValue<std::string>("user/license/license", licenseString);
}

void ApplicationSettings::setLicenseCheck(const std::string& hash)
{
	setValue<std::string>("user/license/check", hash);
}
