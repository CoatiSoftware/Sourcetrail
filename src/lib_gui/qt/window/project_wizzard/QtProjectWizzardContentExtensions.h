#ifndef QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
#define QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtStringListBox;
class SourceGroupSettings;

class QtProjectWizzardContentExtensions
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentExtensions(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettings> m_settings;

	QtStringListBox* m_listBox;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
