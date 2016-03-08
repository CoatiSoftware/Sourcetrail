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

	void hideLanguage();

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual QSize preferredWindowSize() const override;

private:
	QLineEdit* m_projectName;
	QtLocationPicker* m_projectFileLocation;

	QComboBox* m_language;
	QComboBox* m_cppStandard;
	QComboBox* m_cStandard;

	bool m_showLanguage;

private slots:
	void handleSelectionChanged(int index);
};

#endif // QT_PROJECT_WIZZARD_CONTENT_DATA_H
