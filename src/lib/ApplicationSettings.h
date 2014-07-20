#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

#include "Settings.h"
#include "utility/math/Color.h"

class ApplicationSettings: public Settings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();
	~ApplicationSettings();

	int getCodeTabWidth() const;
	void setCodeTabWidth(int codeTabWidth);

	std::string getCodeFontName() const;
	void setCodeFontName(const std::string& codeFontName);

	int getCodeFontSize() const;
	void setCodeFontSize(int codeFontSize);

	Colori getCodeLinkColor() const;
	void setCodeLinkColor(Colori codeLinkColor);

	Colori getCodeActiveLinkColor() const;
	void setCodeActiveLinkColor(Colori codeLinkColor);

private:
	ApplicationSettings();
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
