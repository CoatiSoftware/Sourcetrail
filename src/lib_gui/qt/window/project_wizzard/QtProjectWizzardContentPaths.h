#ifndef QT_PROJECT_WIZZARD_CONTENT_PATHS_H
#define QT_PROJECT_WIZZARD_CONTENT_PATHS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/path_detector/CombinedPathDetector.h"

class QtDirectoryListBox;
class QPushButton;
class QComboBox;

class QtProjectWizzardContentPaths
	: public QtProjectWizzardContent
{
	Q_OBJECT

signals:
	void showSourceFiles();

public:
	QtProjectWizzardContentPaths(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);

	void addDetection(QGridLayout* layout, int row);

	QtDirectoryListBox* m_list;

	QString m_showFilesString;
	std::shared_ptr<CombinedPathDetector> m_pathDetector;

private slots:
	void detectionClicked();

private:
	QString m_titleString;
	QString m_helpString;

	QComboBox* m_detectorBox;
};


class QtProjectWizzardContentPathsSource
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsSource(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual std::vector<std::string> getFileNames() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;
};

class QtProjectWizzardContentPathsSourceJava
	: public QtProjectWizzardContentPathsSource
{
public:
	QtProjectWizzardContentPathsSourceJava(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);
};

class QtProjectWizzardContentPathsCDBHeader
	: public QtProjectWizzardContentPathsSource
{
	Q_OBJECT

public:
	QtProjectWizzardContentPathsCDBHeader(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

private slots:
	void buttonClicked();
	void savedFilesDialog();
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
	QtProjectWizzardContentPathsHeaderSearch(
		std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window, bool isCDB = false);

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
	QtProjectWizzardContentPathsFrameworkSearch(
		std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window, bool isCDB = false);

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


class QtProjectWizzardContentPathsClassJava
	: public QtProjectWizzardContentPaths
{
public:
	QtProjectWizzardContentPathsClassJava(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PATHS_H
