#ifndef QT_PROJECT_WIZZARD_CONTENT_PATHS_H
#define QT_PROJECT_WIZZARD_CONTENT_PATHS_H

#include <QComboBox>
#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtDirectoryListBox;

class QtProjectWizzardContentPaths
	: public QtProjectWizzardContent
{
	Q_OBJECT

signals:
	void showSourceFiles();

public:
	QtProjectWizzardContentPaths(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual QSize preferredWindowSize() const override;

	virtual bool check() override;

protected:
	void setInfo(const QString& title, const QString& description, const QString& help);
	void setTitleString(const QString& title);
	void setDescriptionString(const QString& description);
	void setHelpString(const QString& help);

	void addDetection(QString name, QGridLayout* layout, int row);

	QtDirectoryListBox* m_list;

	QString m_showFilesString;
	QString m_detectionString;

private slots:
	void detectionClicked();

private:
	QString m_titleString;
	QString m_descriptionString;
	QString m_helpString;

	QComboBox* m_detectorBox;
};


class QtProjectWizzardContentPathsSource
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsSource(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual QSize preferredWindowSize() const override;

	virtual void load() override;
	virtual void save() override;

	virtual QStringList getFileNames() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;
};

class QtProjectWizzardContentPathsCDBHeader
	: public QtProjectWizzardContentPathsSource
{
public:
	QtProjectWizzardContentPathsCDBHeader(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);
};


class QtProjectWizzardContentPathsExclude
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsExclude(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	virtual void load() override;
	virtual void save() override;
};


class QtProjectWizzardContentPathsHeaderSearch
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsHeaderSearch(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool isScrollAble() const override;
};

class QtProjectWizzardContentPathsHeaderSearchGlobal
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsHeaderSearchGlobal(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;
};


class QtProjectWizzardContentPathsFrameworkSearch
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsFrameworkSearch(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool isScrollAble() const override;
};

class QtProjectWizzardContentPathsFrameworkSearchGlobal
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsFrameworkSearchGlobal(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PATHS_H
