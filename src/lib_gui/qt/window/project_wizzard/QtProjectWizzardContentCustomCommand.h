#ifndef QT_PROJECT_WIZZARD_CONTENT_CUSTOM_COMMAND_H
#define QT_PROJECT_WIZZARD_CONTENT_CUSTOM_COMMAND_H

#include "QtProjectWizzardContent.h"

class QCheckBox;
class QLineEdit;
class SourceGroupSettingsCustomCommand;

class QtProjectWizzardContentCustomCommand
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCustomCommand(
		std::shared_ptr<SourceGroupSettingsCustomCommand> settings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<SourceGroupSettingsCustomCommand> m_settings;

	QLineEdit* m_customCommand;
	QCheckBox* m_runInParallel;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_CUSTOM_COMMAND_H
