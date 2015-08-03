#ifndef QTPROJECTSETUPSCREEN_H
#define QTPROJECTSETUPSCREEN_H

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

#include "QtSettingsWindow.h"

#include "qt/element/QtDirectoryListBox.h"
#include "utility/file/FilePath.h"

class QtTextLine : public QWidget
{
Q_OBJECT
public:
	QtTextLine(QWidget *parent);
	QString getText();
	void setText(QString text);
private:
	QPushButton* m_button;
	QLineEdit* m_data;
private slots:
	void handleButtonPress();
};

class QtProjectSetupScreen : public QtSettingsWindow
{
Q_OBJECT
public:
	QtProjectSetupScreen(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual void setup();
private slots:
	void handleCreateButtonPress();
	void handleCancelButtonPress();

private:
	QLineEdit* m_projectName;
	QtDirectoryListBox *m_includePaths;
	QtDirectoryListBox *m_sourcePaths;
	QtDirectoryListBox *m_frameworkPaths;
	QtTextLine* m_projectFileLocation;
	std::vector<std::string> compilerFlags;
	std::vector<std::string> sourceExtensions;
	std::vector<std::string> headerExtensions;
};


#endif //QTPROJECTSETUPSCREEN_H
