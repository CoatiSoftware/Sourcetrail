#ifndef QT_PROJECT_WIZZARD_CONTENT_FLAGS_H
#define QT_PROJECT_WIZZARD_CONTENT_FLAGS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtStringListBox;
class SourceGroupSettings;

class QtProjectWizzardContentFlags
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentFlags(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB = false);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettings> m_settings;
	const bool m_isCdb;

	QtStringListBox* m_list;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_FLAGS_H
