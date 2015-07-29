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
	return getValue<std::string>("StartupProject", "");
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPaths() const
{
	return getPathValues("source/HeaderSearchPaths/HeaderSearchPath");
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPaths() const
{
	return getPathValues("source/FrameworkSearchPaths/FrameworkSearchPath");
}

std::vector<std::string> ApplicationSettings::getCompilerFlags() const
{
	std::vector<std::string> defaultValues;
	return getValues("source/CompilerFlags/CompilerFlag", defaultValues);
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
	return getValue<int>("code/TabWidth", 4);
}

void ApplicationSettings::setCodeTabWidth(int codeTabWidth)
{
	setValue<int>("code/TabWidth", codeTabWidth);
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
