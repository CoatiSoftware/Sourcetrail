#ifndef QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
#define QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

#include "qt/element/QtLocationPicker.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtProjectWizzardContentPreferences
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentPreferences(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);
	~QtProjectWizzardContentPreferences();

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private slots:
	void colorSchemeChanged(int index);

private:
	QLineEdit* m_fontFace;
	QComboBox* m_fontSize;
	QComboBox* m_tabWidth;

	QComboBox* m_colorSchemes;
	QLineEdit* m_scrollSpeed;
	std::vector<FilePath> m_colorSchemePaths;
	int m_oldColorSchemeIndex;

	QComboBox* m_threads;
	QCheckBox* m_fatalErrors;

	QtLocationPicker* m_javaPath;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
