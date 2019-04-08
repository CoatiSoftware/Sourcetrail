#ifndef QT_PROJECT_WIZZARD_CONTENT_PATH_H
#define QT_PROJECT_WIZZARD_CONTENT_PATH_H

#include <set>

#include "QtProjectWizzardContent.h"
#include "SingleValueCache.h"

class QCheckBox;
class QComboBox;
class QPushButton;

class QtLocationPicker;
class SourceGroupSettings;
class SourceGroupSettingsCxxCdb;
class SourceGroupSettingsCxxCodeblocks;
class SourceGroupSettingsCxxSonargraph;
class SourceGroupSettingsJavaGradle;
class SourceGroupSettingsPythonEmpty;
class SourceGroupSettingsJavaMaven;
class SourceGroupSettingsWithSonargraphProjectPath;

class QtProjectWizzardContentPath
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentPath(QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);
	void setPlaceholderString(const QString& placeholder);

	void setFileEndings(const std::set<std::wstring>& fileEndings);
	void setAllowEmpty(bool allowEmpty);

	QtLocationPicker* m_picker;

	bool m_makePathRelativeToProjectFileLocation;

private:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;

	QString m_titleString;
	QString m_helpString;
	QString m_placeholderString;
	std::set<std::wstring> m_fileEndings;
	bool m_allowEmpty;
};


class QtProjectWizzardContentPathCDB
	: public QtProjectWizzardContentPath
{
	Q_OBJECT

public:
	QtProjectWizzardContentPathCDB(std::shared_ptr<SourceGroupSettingsCxxCdb> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;
	QString getFileNamesTitle() const override;
	QString getFileNamesDescription() const override;

private slots:
	void pickedPath();

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsCxxCdb> m_settings;
	QLabel* m_fileCountLabel;
	mutable SingleValueCache<std::vector<FilePath>> m_filePaths;
};


class QtProjectWizzardContentCodeblocksProjectPath
	: public QtProjectWizzardContentPath
{
	Q_OBJECT

public:
	QtProjectWizzardContentCodeblocksProjectPath(
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;
	QString getFileNamesTitle() const override;
	QString getFileNamesDescription() const override;

private slots:
	void pickedPath();

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> m_settings;
	QLabel* m_fileCountLabel;
	mutable SingleValueCache<std::vector<FilePath>> m_filePaths;
};


class QtProjectWizzardContentSonargraphProjectPath
	: public QtProjectWizzardContentPath
{
	Q_OBJECT

public:
	QtProjectWizzardContentSonargraphProjectPath(
		std::shared_ptr<SourceGroupSettings> settings,
		std::shared_ptr<SourceGroupSettingsCxxSonargraph> settingsCxxSonargraph,
		std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settingsWithSonargraphProjectPath,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;
	bool check() override;

	std::vector<FilePath> getFilePaths() const override;
	QString getFileNamesTitle() const override;
	QString getFileNamesDescription() const override;

private slots:
	void pickedPath();

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettings> m_settings;
	std::shared_ptr<SourceGroupSettingsCxxSonargraph> m_settingsCxxSonargraph;
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> m_settingsWithSonargraphProjectPath;
	QLabel* m_fileCountLabel;
	mutable SingleValueCache<std::vector<FilePath>> m_filePaths;
};


class QtProjectWizzardContentPathSourceMaven
	: public QtProjectWizzardContentPath
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathSourceMaven(std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
	QCheckBox* m_shouldIndexTests;
};


class QtProjectWizzardContentPathDependenciesMaven
	: public QtProjectWizzardContentPath
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathDependenciesMaven(std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
};


class QtProjectWizzardContentPathSourceGradle
	: public QtProjectWizzardContentPath
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathSourceGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
	QCheckBox* m_shouldIndexTests;
};


class QtProjectWizzardContentPathDependenciesGradle
	: public QtProjectWizzardContentPath
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathDependenciesGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
};


class QtProjectWizzardContentPathPythonEnvironment
	: public QtProjectWizzardContentPath
{
	Q_OBJECT
public:
	QtProjectWizzardContentPathPythonEnvironment(std::shared_ptr<SourceGroupSettingsPythonEmpty> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsPythonEmpty> m_settings;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PATH_H
