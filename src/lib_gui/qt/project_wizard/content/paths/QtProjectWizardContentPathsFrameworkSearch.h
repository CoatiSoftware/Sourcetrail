#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsFrameworkSearch: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsFrameworkSearch(
		std::shared_ptr<SourceGroupSettings> settings,
		QtProjectWizardWindow* window,
		bool indicateAsAdditional = false);

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATHS_FRAMEWORK_SEARCH_H
