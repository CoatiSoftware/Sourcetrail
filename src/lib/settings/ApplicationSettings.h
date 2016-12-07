#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

#include "settings/Settings.h"

class ApplicationSettings
	: public Settings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();

	static const size_t VERSION;

	ApplicationSettings();
	~ApplicationSettings();

	bool load(const FilePath& filePath);

	bool operator==(const ApplicationSettings& other) const;

	int getMaxRecentProjectsCount() const;

	// application
	std::string getFontName() const;
	void setFontName(const std::string& fontName);

	int getFontSize() const;
	void setFontSize(int fontSize);

	FilePath getColorSchemePath() const;
	void setColorSchemePath(const FilePath& colorSchemePath);

	int getFontSizeMax() const;
	void setFontSizeMax(const int fontSizeMax);

	int getFontSizeMin() const;
	void setFontSizeMin(const int fontSizeMin);

	int getFontSizeStd() const;
	void setFontSizeStd(const int fontSizeStd);

	int getWindowBaseWidth() const;
	int getWindowBaseHeight() const;

	float getScrollSpeed() const;
	void setScrollSpeed(float scrollSpeed);

	// logging
	bool getLoggingEnabled() const;
	void setLoggingEnabled(bool loggingEnabled);

	bool getVerboseIndexerLoggingEnabled() const;
	void setVerboseIndexerLoggingEnabled(bool loggingEnabled);

	int getLogFilter() const;
	void setLogFilter(int mask);

	int getStatusFilter() const;
	void setStatusFilter(int mask);

	std::vector<FilePath> getIndexingFilePaths() const;
	bool setIndexingFilePaths(const std::vector<FilePath>& indexingFiles);

	// indexing
	int getIndexerThreadCount() const;
	void setIndexerThreadCount(const int count);

	std::string getJavaPath() const;
	void setJavaPath(const std::string path);

	int getJavaMaximumMemory() const;
	void setJavaMaximumMemory(int size);

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpanded() const;
	bool setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpanded() const;
	bool setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

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

	bool getControlsGraphZoomOnMouseWheel() const;
	void setControlsGraphZoomOnMouseWheel(bool zoomingDefault);

	// license
	std::string getLicenseString() const;
	void setLicenseString(const std::string& licenseString);

	std::string getLicenseCheck() const;
	void setLicenseCheck(const std::string& hash);

private:
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
