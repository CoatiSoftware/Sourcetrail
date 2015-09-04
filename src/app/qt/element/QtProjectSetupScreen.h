#ifndef QT_PROJECT_SETUP_SCREEN_H
#define QT_PROJECT_SETUP_SCREEN_H

#include <QListWidget>
#include <QPushButton>
#include <QWidget>

#include "QtSettingsWindow.h"

#include "utility/file/FilePath.h"

#include "qt/element/QtDirectoryListBox.h"
#include "qt/element/QtLineEdit.h"

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

signals:
	void finished();
	void canceled();

private slots:
	void handleCreateButtonPress();
	void handleCancelButtonPress();
	void handleUpdateButtonPress();

private:
	QLineEdit* m_projectName;
	QtTextLine* m_projectFileLocation;

	QLabel* m_title;

	QPushButton* m_createButton;
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

	QtDirectoryListBox* m_includePaths;
	QtDirectoryListBox* m_sourcePaths;
	QtDirectoryListBox* m_frameworkPaths;
};

#endif //QT_PROJECT_SETUP_SCREEN_H
