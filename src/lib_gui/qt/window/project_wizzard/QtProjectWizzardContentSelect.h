#ifndef QT_PROJECT_WIZZARD_CONTENT_SELECT_H
#define QT_PROJECT_WIZZARD_CONTENT_SELECT_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QButtonGroup;
class SolutionParserManager;

class QtProjectWizzardContentSelect
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSelect(
		std::shared_ptr<ProjectSettings> settings,
		QtProjectWizzardWindow* window,
		std::weak_ptr<SolutionParserManager> solutionParserManager);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void save() override;
	virtual bool check() override;

signals:
	void selected(ProjectType);

private:
	QButtonGroup* m_languages;
	std::map<LanguageType, QButtonGroup*> m_buttons;

	QLabel* m_title;
	QLabel* m_description;

	std::map<ProjectType, std::string> m_projectTypeIconName;
	std::map<ProjectType, std::string> m_projectTypeDescriptions;

	std::weak_ptr<SolutionParserManager> m_solutionParserManager;
	std::vector<std::string> m_solutionDescription;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
