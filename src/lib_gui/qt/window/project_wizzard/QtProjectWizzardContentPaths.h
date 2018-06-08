#ifndef QT_PROJECT_WIZZARD_CONTENT_PATHS_H
#define QT_PROJECT_WIZZARD_CONTENT_PATHS_H

#include <set>

#include "qt/element/QtPathListBox.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/path_detector/CombinedPathDetector.h"

class IncludeDirective;
class QCheckBox;
class QComboBox;
class QPushButton;
class QtPathListDialog;
class SourceGroupSettings;
class SourceGroupSettingsCxxCdb;
class SourceGroupSettingsCxxCodeblocks;
class SourceGroupSettingsCxxSonargraph;

class QtProjectWizzardContentPaths
	: public QtProjectWizzardContent
{
	Q_OBJECT

signals:
	void showSourceFiles();

public:
	QtProjectWizzardContentPaths(
		std::shared_ptr<SourceGroupSettings> settings, 
		QtProjectWizzardWindow* window, 
		QtPathListBox::SelectionPolicyType selectionPolicy,
		bool checkMissingPaths = true);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);

	void addDetection(QGridLayout* layout, int row);

	std::shared_ptr<SourceGroupSettings> m_settings;

	QtPathListBox* m_list;

	QString m_showFilesString;
	std::shared_ptr<CombinedPathDetector> m_pathDetector;

	bool m_makePathsRelativeToProjectFileLocation;

private slots:
	void detectionClicked();

private:
	const QtPathListBox::SelectionPolicyType m_selectionPolicy;
	const bool m_checkMissingPaths;
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

	virtual std::vector<FilePath> getFilePaths() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;
};


class QtProjectWizzardContentIndexedHeaderPaths
	: public QtProjectWizzardContentPaths
{
	Q_OBJECT

public:
	static std::vector<FilePath> getIndexedPathsDerivedFromSonargraphProject(std::shared_ptr<const SourceGroupSettingsCxxSonargraph> settings);
	static std::vector<FilePath> getIndexedPathsDerivedFromCodeblocksProject(std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> settings);
	static std::vector<FilePath> getIndexedPathsDerivedFromCDB(std::shared_ptr<const SourceGroupSettingsCxxCdb> settings);

	QtProjectWizzardContentIndexedHeaderPaths(
		const std::string& projectKindName, std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

	// QtProjectWizzardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool check() override;

private slots:
	void buttonClicked();
	void savedFilesDialog();

private:
	const std::string m_projectKindName;
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
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool indicateAsAdditional = false);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
	virtual void load() override;
	virtual void save() override;
	virtual bool isScrollAble() const override;

private slots:
	void detectIncludesButtonClicked();
	void validateIncludesButtonClicked();
	void finishedSelectDetectIncludesRootPathsDialog();
	void finishedAcceptDetectedIncludePathsDialog();
	void closedPathsDialog();

private:
	void showDetectedIncludesResult(const std::set<FilePath>& detectedHeaderSearchPaths);
	void showValidationResult(const std::vector<IncludeDirective>& unresolvedIncludes);

	QtThreadedFunctor<std::set<FilePath>> m_showDetectedIncludesResultFunctor;
	QtThreadedFunctor<std::vector<IncludeDirective>> m_showValidationResultFunctor;

	std::shared_ptr<QtPathListDialog> m_pathsDialog;
	const bool m_indicateAsAdditional;
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
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool indicateAsAdditional = false);

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
