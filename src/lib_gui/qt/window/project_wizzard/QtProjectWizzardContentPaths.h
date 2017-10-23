#ifndef QT_PROJECT_WIZZARD_CONTENT_PATHS_H
#define QT_PROJECT_WIZZARD_CONTENT_PATHS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/path_detector/CombinedPathDetector.h"

class IncludeDirective;
class QCheckBox;
class QComboBox;
class QPushButton;
class QtDirectoryListBox;
class SourceGroupSettings;
class SourceGroupSettingsCxxCdb;

class QtProjectWizzardContentPaths
	: public QtProjectWizzardContent
{
	Q_OBJECT

signals:
	void showSourceFiles();

public:
	QtProjectWizzardContentPaths(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);

	void addDetection(QGridLayout* layout, int row);

	std::shared_ptr<SourceGroupSettings> m_settings;

	QtDirectoryListBox* m_list;

	QString m_showFilesString;
	std::shared_ptr<CombinedPathDetector> m_pathDetector;

	bool m_makePathsRelativeToProjectFileLocation;

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
	Q_OBJECT
public:
	QtProjectWizzardContentPathsSource(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual std::vector<std::string> getFileNames() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;
};

class QtProjectWizzardContentPathsCDBHeader
	: public QtProjectWizzardContentPathsSource
{
	Q_OBJECT

public:
	static std::vector<FilePath> getTopLevelHeaderSearchPaths(std::shared_ptr<SourceGroupSettingsCxxCdb> settings);

	QtProjectWizzardContentPathsCDBHeader(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;

	virtual bool check() override;

private slots:
	void buttonClicked();
	void savedFilesDialog();
};


class QtProjectWizzardContentPathsExclude
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsExclude(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	virtual void load() override;
	virtual void save() override;
};


class QtProjectWizzardContentPathsHeaderSearch
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsHeaderSearch(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB = false);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
	virtual void load() override;
	virtual void save() override;
	virtual bool isScrollAble() const override;

private slots:
	void validateButtonClicked();

private:
	void showValidationResult(const std::vector<IncludeDirective>& unresolvedIncludes);
	QtThreadedFunctor<std::vector<IncludeDirective>> m_showValidationResultFunctor;

	const bool m_isCdb;
};

class QtProjectWizzardContentPathsHeaderSearchGlobal
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsHeaderSearchGlobal(QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;
};


class QtProjectWizzardContentPathsFrameworkSearch
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsFrameworkSearch(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB = false);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool isScrollAble() const override;
};

class QtProjectWizzardContentPathsFrameworkSearchGlobal
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsFrameworkSearchGlobal(QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;
};

class QtProjectWizzardContentPathsClassJava
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathsClassJava(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
	virtual void load() override;
	virtual void save() override;

private:
	QCheckBox* m_useJreSystemLibraryCheckBox;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PATHS_H
