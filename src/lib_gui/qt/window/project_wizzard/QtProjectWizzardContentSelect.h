#ifndef QT_PROJECT_WIZZARD_CONTENT_SELECT_H
#define QT_PROJECT_WIZZARD_CONTENT_SELECT_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "settings/LanguageType.h"
#include "settings/SourceGroupType.h"

class QButtonGroup;
class SolutionParserManager;

class QtProjectWizzardContentSelect
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSelect(QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void save() override;
	virtual bool check() override;

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

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
