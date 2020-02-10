#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_H

#include "FilePathFilter.h"
#include "QtProjectWizardContentPaths.h"

class IncludeDirective;
class QtPathListDialog;

class QtProjectWizardContentPathsHeaderSearch: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsHeaderSearch(
		std::shared_ptr<SourceGroupSettings> settings,
		QtProjectWizardWindow* window,
		bool indicateAsAdditional = false);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
	virtual void load() override;
	virtual void save() override;

private slots:
	void detectIncludesButtonClicked();
	void validateIncludesButtonClicked();
	void finishedSelectDetectIncludesRootPathsDialog();
	void finishedAcceptDetectedIncludePathsDialog();
	void closedPathsDialog();

private:
	void showDetectedIncludesResult(const std::set<FilePath>& detectedHeaderSearchPaths);
	void showValidationResult(const std::vector<IncludeDirective>& unresolvedIncludes);

	QtThreadedFunctor<std::set<FilePath>> m_showDetectedIncludesResultFunctor;
	QtThreadedFunctor<std::vector<IncludeDirective>> m_showValidationResultFunctor;

	std::shared_ptr<QtPathListDialog> m_pathsDialog;
	const bool m_indicateAsAdditional;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATHS_HEADER_SEARCH_H
