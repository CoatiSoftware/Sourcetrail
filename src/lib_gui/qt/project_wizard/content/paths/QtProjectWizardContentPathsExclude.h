#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_EXCLUDE_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_EXCLUDE_H

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsExclude
	: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsExclude(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window);

	virtual void load() override;
	virtual void save() override;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_EXCLUDE_H
