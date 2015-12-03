#include "settings/ApplicationSettings.h"

std::shared_ptr<ApplicationSettings> ApplicationSettings::s_instance;

std::shared_ptr<ApplicationSettings> ApplicationSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ApplicationSettings>(new ApplicationSettings());
	}

	return s_instance;
}

ApplicationSettings::~ApplicationSettings()
{
}

std::string ApplicationSettings::getStartupProjectFilePath() const
{
	return getValue<std::string>("startup_project", "");
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPaths() const
{
	return getPathValues("source/header_search_paths/header_search_path");
}

bool ApplicationSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("source/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPaths() const
{
	return getPathValues("source/framework_search_paths/framework_search_path");
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

std::string ApplicationSettings::getColorSchemePath() const
{
	return getValue<std::string>("application/color_scheme", "./data/color_schemes/bright.xml");
}

void ApplicationSettings::setColorSchemePath(const std::string& colorSchemePath)
{
	setValue<std::string>("application/color_scheme", colorSchemePath);
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

ApplicationSettings::ApplicationSettings()
{
}

std::vector<FilePath> ApplicationSettings::getRecentProjects() const
{
	std::vector<FilePath> recentProjects;
	std::vector<FilePath> loadedRecentProjects = getPathValues("user/recent_projects/recent_project");

	for (FilePath project: loadedRecentProjects)
	{
		if (project.exists())
		{
			recentProjects.push_back(project);
		}
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