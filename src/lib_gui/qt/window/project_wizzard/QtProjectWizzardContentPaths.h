#ifndef QT_PROJECT_WIZZARD_CONTENT_PATHS_H
#define QT_PROJECT_WIZZARD_CONTENT_PATHS_H

#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtDirectoryListBox;

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


class QtProjectWizzardContentPaths
	: public QtProjectWizzardContent
{
public:
	QtProjectWizzardContentPaths(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	void populateLayout(QVBoxLayout* layout);
	virtual void populateForm(QFormLayout* layout) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual void loadPaths();
	virtual void savePaths();
	virtual bool checkPaths();

protected:
	void setInfo(const QString& title, const QString& description, const QString& help);

	QtDirectoryListBox* m_list;
	QtProjectWizzardContentPaths* m_subPaths;

private:
	QString m_titleString;
	QString m_descriptionString;
	QString m_helpString;
};


class QtProjectWizzardContentPathsSource
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsSource(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void loadPaths() override;
	virtual void savePaths() override;
	virtual bool checkPaths() override;
};


class QtProjectWizzardContentPathsHeaderSearch
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsHeaderSearch(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void loadPaths() override;
	virtual void savePaths() override;
};

class QtProjectWizzardContentPathsHeaderSearchGlobal
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsHeaderSearchGlobal(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void loadPaths() override;
	virtual void savePaths() override;
};


class QtProjectWizzardContentPathsFrameworkSearch
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsFrameworkSearch(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void loadPaths() override;
	virtual void savePaths() override;
};

class QtProjectWizzardContentPathsFrameworkSearchGlobal
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsFrameworkSearchGlobal(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void loadPaths() override;
	virtual void savePaths() override;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PATHS_H
