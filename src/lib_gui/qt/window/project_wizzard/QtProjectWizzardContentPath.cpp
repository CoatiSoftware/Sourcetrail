#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "qt/element/QtLocationPicker.h"
#include "qt/view/QtDialogView.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "settings/SourceGroupSettingsJavaMaven.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "settings/SourceGroupSettingsWithSonargraphProjectPath.h"
#include "utility/file/FileManager.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/sonargraph/SonargraphProject.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityGradle.h"
#include "utility/utilityMaven.h"
#include "Application.h"

QtProjectWizzardContentPath::QtProjectWizzardContentPath(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_makePathRelativeToProjectFileLocation(true)
{
}

void QtProjectWizzardContentPath::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_titleString, m_helpString, layout, row);
	}

	m_picker = new QtLocationPicker(this);
	m_picker->setPickDirectory(true);

	if (m_makePathRelativeToProjectFileLocation)
	{
		m_picker->setRelativeRootDirectory(m_settings->getProjectDirectoryPath());
	}

	layout->addWidget(m_picker, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

bool QtProjectWizzardContentPath::check()
{
	QString error;

	while (true)
	{
		if (m_picker->getText().isEmpty())
		{
			error = "Please define a path at \"" + m_titleString + "\".";
			break;
		}

		FilePath path = m_settings->makePathExpandedAndAbsolute(FilePath(m_picker->getText().toStdWString()));

		if (m_picker->pickDirectory())
		{
			break;
		}

		if (!path.exists())
		{
			error = "The entered path does not exist at \"" + m_titleString + "\".";
			break;
		}

		if (m_fileEndings.find(path.extension()) == m_fileEndings.end())
		{
			error = "The entered path does have a correct file ending at \"" + m_titleString + "\".";
			break;
		}

		break;
	}

	if (error.size())
	{
		QMessageBox msgBox;
		msgBox.setText(error);
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizzardContentPath::setTitleString(const QString& title)
{
	m_titleString = title;
}

void QtProjectWizzardContentPath::setHelpString(const QString& help)
{
	m_helpString = help;
}

void QtProjectWizzardContentPath::setFileEndings(const std::set<std::wstring>& fileEndings)
{
	m_fileEndings = fileEndings;
}


QtProjectWizzardContentPathCDB::QtProjectWizzardContentPathCDB(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Compilation Database (compile_commands.json)");
	setHelpString(
		"Select the compilation database file for the project. Sourcetrail will index your project based on the compile "
		"commands. This file contains using all include paths and compiler flags of these compile commands. The project "
		"will stay up to date with changes in the compilation database on every refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ L".json" });
}

void QtProjectWizzardContentPathCDB::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("JSON Compilation Database (*.json)");
	connect(m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizzardContentPathCDB::pickedPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all include paths and compiler flags from the Compilation Database and stay up-to-date "
		"with changes on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	QLabel* title = createFormLabel("Source Files to Index");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_fileCountLabel = new QLabel("");
	m_fileCountLabel->setWordWrap(true);
	layout->addWidget(m_fileCountLabel, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignTop);
	row++;

	addFilesButton("show source files", layout, row);
	row++;
}

void QtProjectWizzardContentPathCDB::load()
{
	m_filePaths.clear();

	const FilePath projectPath = m_settings->getProjectDirectoryPath();

	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
	{
		m_picker->setText(QString::fromStdWString(cxxSettings->getCompilationDatabasePath().wstr()));

		const std::vector<FilePathFilter> excludeFilters = cxxSettings->getExcludeFiltersExpandedAndAbsolute();
		const FilePath cdbPath = cxxSettings->getCompilationDatabasePathExpandedAndAbsolute();

		if (!cdbPath.empty() && cdbPath.exists())
		{
			std::vector<FilePath> filePaths = IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath);

			for (FilePath& path : filePaths)
			{
				{
					bool excluded = false;
					for (const FilePathFilter& filter : excludeFilters)
					{
						if (filter.isMatching(path))
						{
							excluded = true;
							break;
						}
					}

					if (excluded)
					{
						continue;
					}
				}

				if (projectPath.exists())
				{
					path.makeRelativeTo(projectPath);
				}

				m_filePaths.push_back(path);
			}
		}

		if (m_fileCountLabel)
		{
			m_fileCountLabel->setText("<b>" + QString::number(m_filePaths.size()) + "</b> source files were found in the compilation database.");
		}
	}
}

void QtProjectWizzardContentPathCDB::save()
{
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings);
	if (settings)
	{
		settings->setCompilationDatabasePath(FilePath(m_picker->getText().toStdWString()));
	}
}

std::vector<FilePath> QtProjectWizzardContentPathCDB::getFilePaths() const
{
	const_cast<QtProjectWizzardContentPathCDB*>(this)->load();

	return m_filePaths;
}

QString QtProjectWizzardContentPathCDB::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizzardContentPathCDB::getFileNamesDescription() const
{
	return " source files will be indexed.";
}

void QtProjectWizzardContentPathCDB::pickedPath()
{
	m_window->saveContent();

	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
	{
		const FilePath projectPath = m_settings->getProjectDirectoryPath();

		std::set<FilePath> indexedHeaderPaths;
		for (const FilePath& path : QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromCDB(cdbSettings))
		{
			if (projectPath.contains(path))
			{
				indexedHeaderPaths.insert(path.getRelativeTo(projectPath));
			}
		}
		cdbSettings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));
	}

	m_window->loadContent();
}


QtProjectWizzardContentSonargraphProjectPath::QtProjectWizzardContentSonargraphProjectPath(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Sonargraph Project (system.sonargraph)");
	setHelpString(
		"Select the Sonargraph file for the project. Sourcetrail will index your project based on the settings "
		"this file. It contains using all include paths and compiler flags required. The Sourcetrail project "
		"will stay up to date with changes in the Sonargraph project on every refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ L".sonargraph" });
}

void QtProjectWizzardContentSonargraphProjectPath::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Sonargraph Project (system.sonargraph)");
	connect(m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizzardContentSonargraphProjectPath::pickedPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all settings from the Sonargraph project and stay up-to-date with changes on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	QLabel* title = createFormLabel("Source Files to Index");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_fileCountLabel = new QLabel("");
	m_fileCountLabel->setWordWrap(true);
	layout->addWidget(m_fileCountLabel, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignTop);
	row++;

	addFilesButton("show source files", layout, row);
	row++;
}

void QtProjectWizzardContentSonargraphProjectPath::load()
{
	m_filePaths.clear();

	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithSonargraphProjectPath>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdWString(settings->getSonargraphProjectPath().wstr()));

		const FilePath sonargraphProjectPath = settings->getSonargraphProjectPathExpandedAndAbsolute();
		if (!sonargraphProjectPath.empty() && sonargraphProjectPath.exists())
		{
			if (std::shared_ptr<Sonargraph::Project> sonargraphProject = Sonargraph::Project::load(
				sonargraphProjectPath, m_settings->getLanguage()
			))
			{
				m_filePaths = utility::toVector(sonargraphProject->getAllSourceFilePathsCanonical());
			}
		}

		if (m_fileCountLabel)
		{
			m_fileCountLabel->setText("<b>" + QString::number(m_filePaths.size()) + "</b> source files were found in the Sonargraph project.");
		}
	}
}

void QtProjectWizzardContentSonargraphProjectPath::save()
{
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithSonargraphProjectPath>(m_settings);
	if (settings)
	{
		settings->setSonargraphProjectPath(FilePath(m_picker->getText().toStdWString()));
	}
}

bool QtProjectWizzardContentSonargraphProjectPath::check()
{
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithSonargraphProjectPath>(m_settings);
	if (settings)
	{
		if (std::shared_ptr<Sonargraph::Project> sonargraphProject = Sonargraph::Project::load(
			settings->getSonargraphProjectPathExpandedAndAbsolute(), m_settings->getLanguage()
		))
		{
			if (sonargraphProject->getLoadedModuleCount() == 0)
			{

				QMessageBox msgBox;
				msgBox.setText(QString::fromStdString(
					"The Sonargraph project file doesn't seem to contain any supported " +
					languageTypeToString(m_settings->getLanguage()) + " modules. Please " +
					"consider choosing another project file or removing this sourcegroup."
				));
				msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
				msgBox.setDefaultButton(QMessageBox::Ok);
				return msgBox.exec() == QMessageBox::Ok;
			}
		}
	}
	return true;
}

std::vector<FilePath> QtProjectWizzardContentSonargraphProjectPath::getFilePaths() const
{
	const_cast<QtProjectWizzardContentSonargraphProjectPath*>(this)->load();

	return m_filePaths;
}

QString QtProjectWizzardContentSonargraphProjectPath::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizzardContentSonargraphProjectPath::getFileNamesDescription() const
{
	return " source files will be indexed.";
}

void QtProjectWizzardContentSonargraphProjectPath::pickedPath()
{
	m_window->saveContent();

	if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> sonargraphSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(m_settings))
	{
		const FilePath projectPath = m_settings->getProjectDirectoryPath();

		std::set<FilePath> indexedHeaderPaths;
		for (const FilePath& path : QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromSonargraphProject(sonargraphSettings))
		{
			std::string asdasd = path.str();
			if (projectPath.contains(path))
			{
				indexedHeaderPaths.insert(path.getRelativeTo(projectPath));
			}
		}
		sonargraphSettings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));
	}

	m_window->loadContent();
}


QtProjectWizzardContentPathSourceMaven::QtProjectWizzardContentPathSourceMaven(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Maven Project File (pom.xml)");
	setHelpString(
		"Enter the path to the main pom.xml file of your Maven project.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ L".xml" });
}

void QtProjectWizzardContentPathSourceMaven::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("POM File (pom.xml)");

	QPushButton* filesButton = addFilesButton("show source files", nullptr, row);
	m_shouldIndexTests = new QCheckBox("Should Index Tests");

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(m_shouldIndexTests);
	hlayout->addStretch();
	hlayout->addWidget(filesButton);

	layout->addLayout(hlayout, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentPathSourceMaven::load()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdWString(settings->getMavenProjectFilePath().wstr()));
		m_shouldIndexTests->setChecked(settings->getShouldIndexMavenTests());
	}
}

void QtProjectWizzardContentPathSourceMaven::save()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		settings->setMavenProjectFilePath(FilePath(m_picker->getText().toStdWString()));
		settings->setShouldIndexMavenTests(m_shouldIndexTests->isChecked());
	}
}

std::vector<FilePath> QtProjectWizzardContentPathSourceMaven::getFilePaths() const
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);

	const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
	const FilePath mavenProjectRoot = settings->getMavenProjectFilePathExpandedAndAbsolute().getParentDirectory();

	std::vector<FilePath> list;
	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	ScopedFunctor scopedFunctor([&dialogView](){
		dialogView->hideUnknownProgressDialog();
	});

	std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
	dialogView->showUnknownProgressDialog(L"Preparing Project", L"Maven\nGenerating Source Files");
	const bool success = utility::mavenGenerateSources(mavenPath, mavenProjectRoot);
	if (!success)
	{
		const std::wstring dialogMessage =
			L"Sourcetrail was unable to locate Maven on this machine.\n"
			"Please make sure to provide the correct Maven Path in the preferences.";

		MessageStatus(dialogMessage, true, false).dispatch();

		Application::getInstance()->handleDialog(dialogMessage);
	}
	else
	{
		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Maven\nFetching Source Directories");
		const std::vector<FilePath> sourceDirectories = utility::mavenGetAllDirectoriesFromEffectivePom(
			mavenPath,
			mavenProjectRoot,
			settings->getShouldIndexMavenTests()
		);

		if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings))
		{
			FileManager fileManager;
			fileManager.update(
				sourceDirectories,
				settings->getExcludeFiltersExpandedAndAbsolute(),
				pathSettings->getSourceExtensions()
			);

			const FilePath projectPath = m_settings->getProjectDirectoryPath();

			for (FilePath path : fileManager.getAllSourceFilePaths())
			{
				if (projectPath.exists())
				{
					path.makeRelativeTo(projectPath);
				}

				list.push_back(path);
			}
		}
	}

	return list;
}


QtProjectWizzardContentPathDependenciesMaven::QtProjectWizzardContentPathDependenciesMaven(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Intermediate Dependencies Directory");
	setHelpString(
		"This directory is used to temporarily download and store the dependencies (e.g. .jar files) of the Maven project while it is indexed.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathDependenciesMaven::load()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdWString(settings->getMavenDependenciesDirectory().wstr()));
	}
}

void QtProjectWizzardContentPathDependenciesMaven::save()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		settings->setMavenDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
	}
}


QtProjectWizzardContentPathSourceGradle::QtProjectWizzardContentPathSourceGradle(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Gradle Project File (build.gradle)");
	setHelpString(
		"Enter the path to the main build.gradle file of your Gradle project.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ L".gradle" });
}

void QtProjectWizzardContentPathSourceGradle::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Gradle Build File (build.gradle)");

	QPushButton* filesButton = addFilesButton("show source files", nullptr, row);
	m_shouldIndexTests = new QCheckBox("Should Index Tests");

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(m_shouldIndexTests);
	hlayout->addStretch();
	hlayout->addWidget(filesButton);

	layout->addLayout(hlayout, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentPathSourceGradle::load()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdWString(settings->getGradleProjectFilePath().wstr()));
		m_shouldIndexTests->setChecked(settings->getShouldIndexGradleTests());
	}
}

void QtProjectWizzardContentPathSourceGradle::save()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		settings->setGradleProjectFilePath(FilePath(m_picker->getText().toStdWString()));
		settings->setShouldIndexGradleTests(m_shouldIndexTests->isChecked());
	}
}

std::vector<FilePath> QtProjectWizzardContentPathSourceGradle::getFilePaths() const
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);

	const FilePath gradleProjectRoot = settings->getGradleProjectFilePathExpandedAndAbsolute().getParentDirectory();

	std::vector<FilePath> list;

	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	ScopedFunctor scopedFunctor([&dialogView]() {
		dialogView->hideUnknownProgressDialog();
	});

	std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
	{
		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gradle\nFetching Source Directories");
		const std::vector<FilePath> sourceDirectories = utility::gradleGetAllSourceDirectories(
			gradleProjectRoot,
			settings->getShouldIndexGradleTests()
		);

		FileManager fileManager;
		fileManager.update(
			sourceDirectories,
			settings->getExcludeFiltersExpandedAndAbsolute(),
			settings->getSourceExtensions()
		);

		const FilePath projectPath = m_settings->getProjectDirectoryPath();

		for (FilePath path : fileManager.getAllSourceFilePaths())
		{
			if (projectPath.exists())
			{
				path.makeRelativeTo(projectPath);
			}

			list.push_back(path);
		}
	}
	
	return list;
}


QtProjectWizzardContentPathDependenciesGradle::QtProjectWizzardContentPathDependenciesGradle(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(settings, window)
{
	setTitleString("Intermediate Dependencies Directory");
	setHelpString(
		"This directory is used to temporarily download and store the dependencies (e.g. .jar files) of the Gradle project while it is indexed.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathDependenciesGradle::load()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdWString(settings->getGradleDependenciesDirectory().wstr()));
	}
}

void QtProjectWizzardContentPathDependenciesGradle::save()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		settings->setGradleDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
	}
}
