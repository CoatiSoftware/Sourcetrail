#ifndef QT_PROJECT_SETUP_SCREEN_H
#define QT_PROJECT_SETUP_SCREEN_H

#include <QPushButton>
#include <QWidget>

#include "utility/file/FilePath.h"

#include "qt/element/QtDirectoryListBox.h"
#include "qt/element/QtLineEdit.h"
#include "qt/window/QtSettingsWindow.h"

class QtTextLine
	: public QWidget
{
	Q_OBJECT

public:
	QtTextLine(QWidget *parent);
	QString getText();
	void setText(QString text);

private slots:
	void handleButtonPress();

private:
	QPushButton* m_button;
	QtLineEdit* m_data;
};


class QtProjectSetupScreen
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtProjectSetupScreen(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;

	void clear();

	virtual void setup() override;

	void loadEmpty();
	void loadProjectSettings();

	void projectSetupScreen();

signals:
	void showPreferences();

protected:
	virtual void populateForm(QFormLayout* layout) override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();

	void handleSourcePathHelpPress();
	void handleIncludePathHelpPress();
	void handleFrameworkPathHelpPress();

	void handlePreferencesButtonPress();

private:
	QLineEdit* m_projectName;
	QtTextLine* m_projectFileLocation;

	QtDirectoryListBox* m_includePaths;
	QtDirectoryListBox* m_sourcePaths;
	QtDirectoryListBox* m_frameworkPaths;

	QPushButton* m_preferencesButton;
};

#endif //QT_PROJECT_SETUP_SCREEN_H
