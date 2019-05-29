#ifndef QT_PROJECT_WIZARD_CONTENT_CUSTOM_COMMAND_H
#define QT_PROJECT_WIZARD_CONTENT_CUSTOM_COMMAND_H

#include "QtProjectWizardContent.h"

class QCheckBox;
class QLineEdit;
class SourceGroupSettingsCustomCommand;

class QtProjectWizardContentCustomCommand
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentCustomCommand(
		std::shared_ptr<SourceGroupSettingsCustomCommand> settings,
		QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<SourceGroupSettingsCustomCommand> m_settings;

	QLineEdit* m_customCommand;
	QCheckBox* m_runInParallel;
};

#endif // QT_PROJECT_WIZARD_CONTENT_CUSTOM_COMMAND_H
