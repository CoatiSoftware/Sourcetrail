#ifndef QT_PROJECT_WIZARD_CONTENT_PREFERENCES_H
#define QT_PROJECT_WIZARD_CONTENT_PREFERENCES_H

#include <QComboBox>

#include "CombinedPathDetector.h"
#include "QtLocationPicker.h"
#include "QtPathListBox.h"
#include "QtProjectWizardContent.h"

class QCheckBox;
class QFontComboBox;
class QLabel;
class QLineEdit;


class QtComboBoxPlaceHolder: public QComboBox
{
	Q_OBJECT

signals:
	void opened();

public:
	virtual void showPopup()
	{
		emit opened();
	}
};


class QtProjectWizardContentPreferences: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentPreferences(QtProjectWizardWindow* window);
	~QtProjectWizardContentPreferences();

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private slots:
	void colorSchemeChanged(int index);
	void javaPathDetectionClicked();
	void jreSystemLibraryPathsDetectionClicked();
	void mavenPathDetectionClicked();
	void loggingEnabledChanged();
	void indexerThreadsChanges(int index);
	void uiAutoScalingChanges(int index);
	void uiScaleFactorChanges(int index);

private:
	void addJavaPathDetection(QGridLayout* layout, int& row);
	void addJreSystemLibraryPathsDetection(QGridLayout* layout, int& row);
	void addMavenPathDetection(QGridLayout* layout, int& row);

	void addTitle(const QString& title, QGridLayout* layout, int& row);
	void addLabel(const QString& label, QGridLayout* layout, int row);
	void addWidget(
		QWidget* widget, QGridLayout* layout, int row, Qt::Alignment widgetAlignment = Qt::Alignment());
	void addLabelAndWidget(
		const QString& label,
		QWidget* widget,
		QGridLayout* layout,
		int row,
		Qt::Alignment widgetAlignment = Qt::Alignment());
	void addGap(QGridLayout* layout, int& row);

	QCheckBox* addCheckBox(
		const QString& label,
		const QString& text,
		const QString& helpText,
		QGridLayout* layout,
		int& row);
	QComboBox* addComboBox(const QString& label, const QString& helpText, QGridLayout* layout, int& row);
	QComboBox* addComboBoxWithWidgets(
		const QString& label,
		const QString& helpText,
		std::vector<QWidget*> widgets,
		QGridLayout* layout,
		int& row);
	QComboBox* addComboBox(
		const QString& label, int min, int max, const QString& helpText, QGridLayout* layout, int& row);
	QComboBox* addComboBoxWithWidgets(
		const QString& label,
		int min,
		int max,
		const QString& helpText,
		std::vector<QWidget*> widgets,
		QGridLayout* layout,
		int& row);
	QLineEdit* addLineEdit(const QString& label, const QString& helpText, QGridLayout* layout, int& row);

	QFontComboBox* m_fontFace;
	QtComboBoxPlaceHolder* m_fontFacePlaceHolder;

	QComboBox* m_fontSize;
	QComboBox* m_tabWidth;
	QComboBox* m_textEncoding;

	QComboBox* m_colorSchemes;
	std::vector<FilePath> m_colorSchemePaths;
	int m_oldColorSchemeIndex;
	int m_newColorSchemeIndex;

	QCheckBox* m_useAnimations;
	QCheckBox* m_showBuiltinTypes;
	QCheckBox* m_showDirectoryInCode;

	QComboBox* m_screenAutoScaling;
	QLabel* m_screenAutoScalingInfoLabel;

	QComboBox* m_screenScaleFactor;
	QLabel* m_screenScaleFactorInfoLabel;

	QLineEdit* m_scrollSpeed;
	QCheckBox* m_graphZooming;

	QCheckBox* m_loggingEnabled;
	QCheckBox* m_verboseIndexerLoggingEnabled;
	QtLocationPicker* m_logPath;

	QLineEdit* m_sourcetrailPort;
	QLineEdit* m_pluginPort;

	QComboBox* m_threads;
	QLabel* m_threadsInfoLabel;

	QCheckBox* m_multiProcessIndexing;

	std::shared_ptr<CombinedPathDetector> m_javaPathDetector;
	std::shared_ptr<CombinedPathDetector> m_jreSystemLibraryPathsDetector;
	std::shared_ptr<CombinedPathDetector> m_mavenPathDetector;

	QComboBox* m_javaPathDetectorBox;
	QComboBox* m_jreSystemLibraryPathsDetectorBox;
	QComboBox* m_mavenPathDetectorBox;
	QtLocationPicker* m_javaPath;
	QtPathListBox* m_jreSystemLibraryPaths;
	QtLocationPicker* m_mavenPath;

	QCheckBox* m_pythonPostProcessing;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PREFERENCES_H
