#ifndef QT_PROJECT_WIZZARD_CONTENT_DATA_H
#define QT_PROJECT_WIZZARD_CONTENT_DATA_H

#include <QComboBox>
#include <QLabel>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/element/QtLocationPicker.h"

class ProjectSettings;
class SourceGroupSettings;

class QtProjectWizzardContentData
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentData(
		std::shared_ptr<ProjectSettings> projectSettings,
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
		QtProjectWizzardWindow* window,
		bool disableNameEditing = false);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

protected:
	virtual void addNameAndLocation(QGridLayout* layout, int& row);
	virtual void addLanguageAndStandard(QGridLayout* layout, int& row);
	virtual void addBuildFilePicker(QGridLayout* layout, int& row, const QString& name, const QString& filter);

	std::shared_ptr<ProjectSettings> m_projectSettings;
	std::shared_ptr<SourceGroupSettings> m_sourceGroupSettings;

	bool m_disableNameEditing;
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
	QtProjectWizzardContentDataCDB(
		std::shared_ptr<ProjectSettings> projectSettings,
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
		QtProjectWizzardWindow* window,
		bool disableNameEditing = false);

	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private slots:
	void pickedCDBPath();
};


class QtProjectWizzardContentDataCDBVS
	: public QtProjectWizzardContentDataCDB
{
	Q_OBJECT

public:
	QtProjectWizzardContentDataCDBVS(
		std::shared_ptr<ProjectSettings> projectSettings,
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

private slots:
	void handleVSCDBClicked();
};

#endif // QT_PROJECT_WIZZARD_CONTENT_DATA_H
