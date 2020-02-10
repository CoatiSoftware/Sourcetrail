#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_SOURCE_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_SOURCE_H

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsSource: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsSource(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool check() override;

	virtual std::vector<FilePath> getFilePaths() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATHS_SOURCE_H
