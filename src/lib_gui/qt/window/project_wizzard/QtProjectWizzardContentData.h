#ifndef QT_PROJECT_WIZZARD_CONTENT_DATA_H
#define QT_PROJECT_WIZZARD_CONTENT_DATA_H

#include <QComboBox>
#include <QLabel>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/element/QtLocationPicker.h"

class QtProjectWizzardContentData
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentData(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual QSize preferredWindowSize() const override;

protected:
	virtual void addNameAndLocation(QGridLayout* layout, int& row);
	virtual void addLanguageAndStandard(QGridLayout* layout, int& row);
	virtual void addBuildFilePicker(QGridLayout* layout, int& row, const QString& name, const QString& filter);

	QLineEdit* m_projectName;
	QtLocationPicker* m_projectFileLocation;

	QLabel* m_language;
	QComboBox* m_standard;

	QtLocationPicker* m_buildFilePicker;
};


class QtProjectWizzardContentDataCDB
	: public QtProjectWizzardContentData
{
	Q_OBJECT

public:
	QtProjectWizzardContentDataCDB(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private slots:
	void refreshClicked();
};

#endif // QT_PROJECT_WIZZARD_CONTENT_DATA_H
