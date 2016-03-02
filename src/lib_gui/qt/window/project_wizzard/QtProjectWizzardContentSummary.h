#ifndef QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H
#define QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentBuildFile.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

class QtProjectWizzardContentSummary
	: public QtProjectWizzardContent
{
public:
	QtProjectWizzardContentSummary(ProjectSettings* settings, QtProjectWizzardWindow* window);

	QtProjectWizzardContentBuildFile* contentBuildFile();
	QtProjectWizzardContentPathsSource* contentPathsSource();

protected:
	// QtProjectContentWindow implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void windowReady() override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual bool isScrollAble() const override;

private:
	QtProjectWizzardContentData* m_data;
	QtProjectWizzardContentBuildFile* m_buildFile;
	QtProjectWizzardContentPathsSource* m_source;
	QtProjectWizzardContentSimple* m_simple;
	QtProjectWizzardContentPathsHeaderSearch* m_headerSearch;
	QtProjectWizzardContentPathsFrameworkSearch* m_frameworkSearch;
	QtProjectWizzardContentFlags* m_compilerFlags;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SUMMARY_H
