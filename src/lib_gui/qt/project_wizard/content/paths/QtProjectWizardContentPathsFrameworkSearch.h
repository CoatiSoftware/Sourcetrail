#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsFrameworkSearch
	: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsFrameworkSearch(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window, bool indicateAsAdditional = false);

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool isScrollAble() const override;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H
