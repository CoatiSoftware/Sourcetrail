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

	std::string getStartupProjectFilePath() const;

	// source
	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<std::string> getCompilerFlags() const;

	// application
	std::string getFontName() const;
	void setFontName(const std::string& fontName);

	int getFontSize() const;
	void setFontSize(int fontSize);

	std::string getColorSchemePath() const;
	void setColorSchemePath(const std::string& colorSchemePath);

	//recent projects
	std::vector<std::string> getRecentProjects() const;
	bool setRecentProjects(const std::vector<std::string>& recentProjects);

	// code
	int getCodeTabWidth() const;
	void setCodeTabWidth(int codeTabWidth);

	int getCodeSnippetSnapRange() const;
	void setCodeSnippetSnapRange(int range);

	int getCodeSnippetExpandRange() const;
	void setCodeSnippetExpandRange(int range);

private:
	ApplicationSettings();
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
