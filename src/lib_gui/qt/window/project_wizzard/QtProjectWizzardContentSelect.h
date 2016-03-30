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
	enum ProjectType : int
	{
		PROJECT_EMPTY = 0,
		PROJECT_CDB = 1,
		PROJECT_VS = 2
	};

	QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window, std::weak_ptr<SolutionParserManager> solutionParserManager);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout) override;

	virtual void save() override;
	virtual bool check() override;

	virtual QSize preferredWindowSize() const override;

signals:
	void selected(QtProjectWizzardContentSelect::ProjectType);

	// void selected(unsigned int type);

private:
	QButtonGroup* m_languages;
	QButtonGroup* m_buttons;

	QLabel* m_title;
	QLabel* m_description;

	std::vector<std::string> m_projectTypeNames;
	std::vector<std::string> m_projectTypeDescriptions;
	std::vector<std::string> m_projectTypeIconPaths;

	std::weak_ptr<SolutionParserManager> m_solutionParserManager;
	std::vector<std::string> m_solutionDescription;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
