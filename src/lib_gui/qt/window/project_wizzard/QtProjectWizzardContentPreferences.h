#ifndef QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
#define QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

class QtProjectWizzardContentPreferences
	: public QtProjectWizzardContent
{
public:
	QtProjectWizzardContentPreferences(ProjectSettings* settings, QtProjectWizzardWindow* window);

protected:
	// QtProjectContentWindow implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void windowReady() override;

	virtual void load() override;
	virtual void save() override;

	virtual bool isScrollAble() const override;

private:
	QtProjectWizzardContentPathsHeaderSearchGlobal* m_headerSearch;
	QtProjectWizzardContentPathsFrameworkSearchGlobal* m_frameworkSearch;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
