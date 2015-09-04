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

	virtual void setup() override;

	void loadEmpty();
	void loadProjectSettings();

protected:
	virtual void populateForm(QFormLayout* layout);

private slots:
	void handleCancelButtonPress();
	void handleDoneButtonPress();

private:
	QLineEdit* m_projectName;
	QtTextLine* m_projectFileLocation;

	QPushButton* m_createButton;
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

	QtDirectoryListBox* m_includePaths;
	QtDirectoryListBox* m_sourcePaths;
	QtDirectoryListBox* m_frameworkPaths;
};

#endif //QT_PROJECT_SETUP_SCREEN_H
