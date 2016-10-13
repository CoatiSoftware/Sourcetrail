#ifndef QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
#define QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

#include "qt/element/QtFontPicker.h"
#include "qt/element/QtLocationPicker.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/path_detector/CombinedPathDetector.h"

class QtProjectWizzardContentPreferences
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentPreferences(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);
	~QtProjectWizzardContentPreferences();

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private slots:
	void colorSchemeChanged(int index);
	void javaPathDetectionClicked();

private:
	void addJavaPathDetection(QGridLayout* layout, int& row);

	QtFontPicker* m_fontFace;
	QComboBox* m_fontSize;
	QComboBox* m_tabWidth;

	QComboBox* m_colorSchemes;
	std::vector<FilePath> m_colorSchemePaths;
	int m_oldColorSchemeIndex;

	QLineEdit* m_scrollSpeed;
	QCheckBox* m_loggingEnabled;

	QLineEdit* m_coatiPort;
	QLineEdit* m_pluginPort;

	QComboBox* m_threads;

	std::shared_ptr<CombinedPathDetector> m_javaPathDetector;
	QComboBox* m_javaPathDetectorBox;
	QtLocationPicker* m_javaPath;
	QLineEdit* m_jvmMaximumMemory;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
