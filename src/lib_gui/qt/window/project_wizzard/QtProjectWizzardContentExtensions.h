#ifndef QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
#define QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H

#include <memory>

#include "QtProjectWizzardContent.h"

class QtStringListBox;
class SourceGroupSettingsWithSourceExtensions;

class QtProjectWizzardContentExtensions
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentExtensions(
		std::shared_ptr<SourceGroupSettingsWithSourceExtensions> settings, 
		QtProjectWizzardWindow* window
	);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettingsWithSourceExtensions> m_settings;

	QtStringListBox* m_listBox;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
