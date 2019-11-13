#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_GLOBAL_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_GLOBAL_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsFrameworkSearchGlobal
	: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsFrameworkSearchGlobal(QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_GLOBAL_H
