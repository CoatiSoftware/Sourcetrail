#ifndef QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
#define QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QCheckBox;
class QButtonGroup;
class SourceGroupSettings;

class QtProjectWizzardContentSimple
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSimple(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettings> m_settings;

	QCheckBox* m_checkBox;
	QLabel* m_title;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
