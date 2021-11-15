#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

#include "GroupType.h"
#include "Settings.h"

class TimeStamp;
class Version;

class ApplicationSettings: public Settings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();

	static const size_t VERSION;

	ApplicationSettings() = default;

	bool load(const FilePath& filePath, bool readOnly = false);

	bool operator==(const ApplicationSettings& other) const;

	size_t getMaxRecentProjectsCount() const;

	// application
	std::string getFontName() const;
	void setFontName(const std::string& fontName);

	int getFontSize() const;
	void setFontSize(int fontSize);

	std::string getTextEncoding() const;
	void setTextEncoding(const std::string& textEncoding);

	std::wstring getColorSchemeName() const;
	FilePath getColorSchemePath() const;
	void setColorSchemeName(const std::wstring& colorSchemeName);

	int getFontSizeMax() const;
	void setFontSizeMax(const int fontSizeMax);

	int getFontSizeMin() const;
	void setFontSizeMin(const int fontSizeMin);

	int getFontSizeStd() const;
	void setFontSizeStd(const int fontSizeStd);

	bool getUseAnimations() const;
	void setUseAnimations(bool useAnimations);

	bool getShowBuiltinTypesInGraph() const;
	void setShowBuiltinTypesInGraph(bool showBuiltinTypes);

	bool getShowDirectoryInCodeFileTitle() const;
	void setShowDirectoryInCodeFileTitle(bool showDirectory);

	int getWindowBaseWidth() const;
	int getWindowBaseHeight() const;

	float getScrollSpeed() const;
	void setScrollSpeed(float scrollSpeed);

	bool getGraphControlsVisible() const;
	void setGraphControlsVisible(bool visible);

	GroupType getGraphGrouping() const;
	void setGraphGrouping(GroupType type);

	// screen
	int getScreenAutoScaling() const;
	void setScreenAutoScaling(int autoScaling);

	float getScreenScaleFactor() const;
	void setScreenScaleFactor(float scaleFactor);

	// logging
	bool getLoggingEnabled() const;
	void setLoggingEnabled(bool loggingEnabled);

	bool getVerboseIndexerLoggingEnabled() const;
	void setVerboseIndexerLoggingEnabled(bool loggingEnabled);

	FilePath getLogDirectoryPath() const;
	void setLogDirectoryPath(const FilePath& path);

	int getLogFilter() const;
	void setLogFilter(int mask);

	int getStatusFilter() const;
	void setStatusFilter(int mask);

	// indexing
	int getIndexerThreadCount() const;
	void setIndexerThreadCount(const int count);

	bool getMultiProcessIndexingEnabled() const;
	void setMultiProcessIndexingEnabled(bool enabled);

	FilePath getJavaPath() const;
	void setJavaPath(const FilePath& path);

	bool getHasPrefilledJavaPath() const;
	void setHasPrefilledJavaPath(bool v);

	int getJavaMaximumMemory() const;
	void setJavaMaximumMemory(int size);

	std::vector<FilePath> getJreSystemLibraryPaths() const;
	std::vector<FilePath> getJreSystemLibraryPathsExpanded() const;
	bool setJreSystemLibraryPaths(const std::vector<FilePath>& jreSystemLibraryPaths);

	bool getHasPrefilledJreSystemLibraryPaths() const;
	void setHasPrefilledJreSystemLibraryPaths(bool v);

	FilePath getMavenPath() const;
	void setMavenPath(const FilePath& path);

	bool getHasPrefilledMavenPath() const;
	void setHasPrefilledMavenPath(bool v);

	bool getPythonPostProcessingEnabled() const;
	void setPythonPostProcessingEnabled(bool enabled);

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpanded() const;
	bool setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	bool getHasPrefilledHeaderSearchPaths() const;
	void setHasPrefilledHeaderSearchPaths(bool v);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpanded() const;
	bool setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	bool getHasPrefilledFrameworkSearchPaths() const;
	void setHasPrefilledFrameworkSearchPaths(bool v);

	// code
	int getCodeTabWidth() const;
	void setCodeTabWidth(int codeTabWidth);

	int getCodeSnippetSnapRange() const;
	void setCodeSnippetSnapRange(int range);

	int getCodeSnippetExpandRange() const;
	void setCodeSnippetExpandRange(int range);

	bool getCodeViewModeSingle() const;
	void setCodeViewModeSingle(bool enabled);

	// user
	std::vector<FilePath> getRecentProjects() const;
	bool setRecentProjects(const std::vector<FilePath>& recentProjects);

	bool getSeenErrorHelpMessage() const;
	void setSeenErrorHelpMessage(bool seen);

	FilePath getLastFilepickerLocation() const;
	void setLastFilepickerLocation(const FilePath& path);

	float getGraphZoomLevel() const;
	void setGraphZoomLevel(float zoomLevel);

	// network
	int getPluginPort() const;
	void setPluginPort(const int pluginPort);

	int getSourcetrailPort() const;
	void setSourcetrailPort(const int sourcetrailPort);

	// controls
	int getControlsMouseBackButton() const;
	int getControlsMouseForwardButton() const;

	bool getControlsGraphZoomOnMouseWheel() const;
	void setControlsGraphZoomOnMouseWheel(bool zoomingDefault);

private:
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif	  // APPLICATION_SETTINGS_H
