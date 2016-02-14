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
	QtProjectWizzardContentData(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void populateForm(QFormLayout* layout) override;
	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	QLineEdit* m_projectName;
	QtLocationPicker* m_projectFileLocation;

	QComboBox* m_language;
	QComboBox* m_cppStandard;
	QComboBox* m_cStandard;

	QLabel* m_cppStandardLabel;
	QLabel* m_cStandardLabel;

private slots:
	void handleSelectionChanged(int index);
};

#endif // QT_PROJECT_WIZZARD_CONTENT_DATA_H
