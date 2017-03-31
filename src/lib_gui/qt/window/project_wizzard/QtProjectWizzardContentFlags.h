#ifndef QT_PROJECT_WIZZARD_CONTENT_FLAGS_H
#define QT_PROJECT_WIZZARD_CONTENT_FLAGS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtDirectoryListBox;

class QtProjectWizzardContentFlags
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentFlags(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettings> m_settings;

	QtDirectoryListBox* m_list;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_FLAGS_H
