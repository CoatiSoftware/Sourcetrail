#ifndef QT_PROJECT_WIZARD_CONTENT_SELECT_H
#define QT_PROJECT_WIZARD_CONTENT_SELECT_H

#include "LanguageType.h"
#include "QtProjectWizardContent.h"
#include "SourceGroupType.h"

class QButtonGroup;
class SolutionParserManager;

class QtProjectWizardContentSelect: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentSelect(QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void save() override;
	bool check() override;

signals:
	void selected(SourceGroupType);

private:
	QButtonGroup* m_languages;
	std::map<LanguageType, QButtonGroup*> m_buttons;

	QLabel* m_title;
	QLabel* m_description;

	std::map<SourceGroupType, std::wstring> m_sourceGroupTypeIconName;
	std::map<SourceGroupType, std::string> m_sourceGroupTypeDescriptions;

	std::vector<std::string> m_solutionDescription;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_SELECT_H
