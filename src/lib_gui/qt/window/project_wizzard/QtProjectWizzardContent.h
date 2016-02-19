#ifndef QT_PROJECT_WIZZARD_CONTENT_H
#define QT_PROJECT_WIZZARD_CONTENT_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "settings/ProjectSettings.h"


class QtHelpButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtHelpButton(const QString& helpText, QWidget* parent = nullptr);

private slots:
	void handleHelpPress();

private:
	QString m_helpText;
};


class QtProjectWizzardContent
	: public QWidget
{
	Q_OBJECT

public:
	QtProjectWizzardContent(ProjectSettings* settings, QtProjectWizzardWindow* window);

	virtual void populateWindow(QWidget* widget);
	virtual void populateWindow(QGridLayout* layout);
	virtual void populateForm(QGridLayout* layout, int& row);
	virtual void windowReady();

	virtual void load();
	virtual void save();
	virtual bool check();

	virtual bool isScrollAble() const;

	virtual QSize preferredWindowSize() const;

protected:
	QLabel* createFormLabel(QString name) const;
	QToolButton* createProjectButton(QString name, QString iconPath) const;

	QtHelpButton* addHelpButton(QString helpString, QGridLayout* layout, int row) const;

	ProjectSettings* m_settings;
	QtProjectWizzardWindow* m_window;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_H
