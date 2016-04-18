#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

#include "settings/Settings.h"

class ApplicationSettings
	: public Settings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();
	~ApplicationSettings();

	int getMaxRecentProjectsCount() const;

	// source
	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpanded() const;
	bool setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpanded() const;
	bool setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	std::vector<std::string> getCompilerFlags() const;

	// application
	std::string getFontName() const;
	void setFontName(const std::string& fontName);

	int getFontSize() const;
	void setFontSize(int fontSize);

	std::string getColorSchemePath() const;
	void setColorSchemePath(const std::string& colorSchemePath);

	int getFontSizeMax() const;
	void setFontSizeMax(const int fontSizeMax);

	int getFontSizeMin() const;
	void setFontSizeMin(const int fontSizeMin);

	int getFontSizeStd() const;
	void setFontSizeStd(const int fontSizeStd);

	// code
	int getCodeTabWidth() const;
	void setCodeTabWidth(int codeTabWidth);

	int getCodeSnippetSnapRange() const;
	void setCodeSnippetSnapRange(int range);

	int getCodeSnippetExpandRange() const;
	void setCodeSnippetExpandRange(int range);

	// user
	std::vector<FilePath> getRecentProjects() const;
	bool setRecentProjects(const std::vector<FilePath>& recentProjects);

	// network
	int getPluginPort() const;
	void setPluginPort(const int pluginPort);

	int getCoatiPort() const;
	void setCoatiPort(const int coatiPort);

	// controls
	int getControlsMouseBackButton() const;
	int getControlsMouseForwardButton() const;

	// license
	std::string getLicenseString() const;
	void setLicenseString(const std::string& licenseString);

	std::string getLicenseCheck() const;
	void setLicenseCheck(const std::string& hash);

private:
	ApplicationSettings();
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
