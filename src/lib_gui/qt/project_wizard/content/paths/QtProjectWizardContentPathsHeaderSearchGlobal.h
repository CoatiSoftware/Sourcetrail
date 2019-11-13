#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_GLOBAL_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_GLOBAL_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPaths.h"

class QtProjectWizardContentPathsHeaderSearchGlobal
	: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsHeaderSearchGlobal(QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool check() override;

protected:
	virtual void detectedPaths(const std::vector<FilePath>& paths) override;

private:
	void setPaths(const std::vector<FilePath>& paths);
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_GLOBAL_H
