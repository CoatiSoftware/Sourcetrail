#ifndef QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_DATA_H
#define QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_DATA_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QLineEdit;
class SourceGroupSettings;

class QtProjectWizzardContentSourceGroupData
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSourceGroupData(
		std::shared_ptr<SourceGroupSettings> settings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

signals:
	void nameUpdated(QString);

private slots:
	void editedName(QString name);

private:
	std::shared_ptr<SourceGroupSettings> m_settings;

	QLineEdit* m_name;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SOURCE_GROUP_DATA_H
