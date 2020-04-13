#ifndef QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_DATA_H
#define QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_DATA_H

#include "QtProjectWizardContent.h"
#include "../../../../lib/settings/source_group/SourceGroupStatusType.h"

class QCheckBox;
class QLineEdit;
class SourceGroupSettings;

class QtProjectWizardContentSourceGroupData: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentSourceGroupData(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

signals:
	void nameUpdated(QString);
	void statusUpdated(SourceGroupStatusType);

private slots:
	void editedName(QString name);
	void changedStatus(bool checked);

private:
	std::shared_ptr<SourceGroupSettings> m_settings;

	QLineEdit* m_name;
	QCheckBox* m_status;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_DATA_H
