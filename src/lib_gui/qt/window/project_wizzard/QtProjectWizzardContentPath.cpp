#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Application.h"
#include "qt/element/QtLocationPicker.h"
#include "qt/view/QtDialogView.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsJavaMaven.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "utility/file/FileManager.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ScopedFunctor.h"
#include "utility/utilityGradle.h"
#include "utility/utilityMaven.h"

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
		addHelpButton(m_helpString, layout, row);
	}

	m_picker = new QtLocationPicker(this);
	m_picker->setPickDirectory(true);

	if (m_makePathRelativeToProjectFileLocation)
	{
		m_picker->setRelativeRootDirectory(m_settings->getProjectFileLocation());
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

		FilePath path = m_settings->makePathExpandedAndAbsolute(FilePath(m_picker->getText().toStdString()));

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

void QtProjectWizzardContentPath::setFileEndings(const std::set<std::string>& fileEndings)
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
		"Sourcetrail will use all include paths and compiler flags from the compilation database and stay up-to-date "
		"with changes on refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ ".json" });
}

void QtProjectWizzardContentPathCDB::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("JSON Compilation Database (*.json)");
	connect(m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizzardContentPathCDB::pickedCDBPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all include paths and compiler flags from the compilation database and stay up-to-date "
		"with changes on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentPathCDB::load()
{
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings);
	if (settings)
	{
		m_picker->setText(QString::fromStdString(settings->getCompilationDatabasePath().str()));
	}
}

void QtProjectWizzardContentPathCDB::save()
{
	std::shared_ptr<SourceGroupSettingsCxxCdb> m_settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings);
	if (m_settings)
	{
		m_settings->setCompilationDatabasePath(FilePath(m_picker->getText().toStdString()));
	}
}

void QtProjectWizzardContentPathCDB::pickedCDBPath()
{
	m_window->saveContent();
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
	setFileEndings({ ".xml" });
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
		m_picker->setText(QString::fromStdString(settings->getMavenProjectFilePath().str()));
		m_shouldIndexTests->setChecked(settings->getShouldIndexMavenTests());
	}
}

void QtProjectWizzardContentPathSourceMaven::save()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		settings->setMavenProjectFilePath(FilePath(m_picker->getText().toStdString()));
		settings->setShouldIndexMavenTests(m_shouldIndexTests->isChecked());
	}
}

std::vector<std::string> QtProjectWizzardContentPathSourceMaven::getFileNames() const
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);

	const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
	const FilePath mavenProjectRoot = settings->getMavenProjectFilePathExpandedAndAbsolute().parentDirectory();

	std::vector<std::string> list;
	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	ScopedFunctor scopedFunctor([&dialogView](){
		dialogView->hideUnknownProgressDialog();
	});

	std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
	dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nGenerating Source Files");
	const bool success = utility::mavenGenerateSources(mavenPath, mavenProjectRoot);
	if (!success)
	{
		const std::string dialogMessage =
			"Sourcetrail was unable to locate Maven on this machine.\n"
			"Please make sure to provide the correct Maven Path in the preferences.";

		MessageStatus(dialogMessage, true, false).dispatch();

		Application::getInstance()->handleDialog(dialogMessage);
	}
	else
	{
		dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nFetching Source Directories");
		const std::vector<FilePath> sourceDirectories = utility::mavenGetAllDirectoriesFromEffectivePom(
			mavenPath,
			mavenProjectRoot,
			settings->getShouldIndexMavenTests()
		);

		FileManager fileManager;
		fileManager.update(
			sourceDirectories,
			m_settings->getExcludePathsExpandedAndAbsolute(),
			m_settings->getSourceExtensions()
		);

		const FilePath projectPath = m_settings->getProjectFileLocation();

		for (FilePath path: fileManager.getAllSourceFilePaths())
		{
			if (projectPath.exists())
			{
				path = path.relativeTo(projectPath);
			}

			list.push_back(path.str());
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
		m_picker->setText(QString::fromStdString(settings->getMavenDependenciesDirectory().str()));
	}
}

void QtProjectWizzardContentPathDependenciesMaven::save()
{
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_settings);
	if (settings)
	{
		settings->setMavenDependenciesDirectory(FilePath(m_picker->getText().toStdString()));
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
	setFileEndings({ ".gradle" });
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
		m_picker->setText(QString::fromStdString(settings->getGradleProjectFilePath().str()));
		m_shouldIndexTests->setChecked(settings->getShouldIndexGradleTests());
	}
}

void QtProjectWizzardContentPathSourceGradle::save()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		settings->setGradleProjectFilePath(FilePath(m_picker->getText().toStdString()));
		settings->setShouldIndexGradleTests(m_shouldIndexTests->isChecked());
	}
}

std::vector<std::string> QtProjectWizzardContentPathSourceGradle::getFileNames() const
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);

	const FilePath gradleProjectRoot = settings->getGradleProjectFilePathExpandedAndAbsolute().parentDirectory();

	std::vector<std::string> list;

	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	ScopedFunctor scopedFunctor([&dialogView]() {
		dialogView->hideUnknownProgressDialog();
	});

	std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
	{
		dialogView->showUnknownProgressDialog("Preparing Project", "Gradle\nFetching Source Directories");
		const std::vector<FilePath> sourceDirectories = utility::gradleGetAllSourceDirectories(
			gradleProjectRoot,
			settings->getShouldIndexGradleTests()
		);

		FileManager fileManager;
		fileManager.update(
			sourceDirectories,
			m_settings->getExcludePathsExpandedAndAbsolute(),
			m_settings->getSourceExtensions()
		);

		const FilePath projectPath = m_settings->getProjectFileLocation();

		for (FilePath path : fileManager.getAllSourceFilePaths())
		{
			if (projectPath.exists())
			{
				path = path.relativeTo(projectPath);
			}

			list.push_back(path.str());
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
		m_picker->setText(QString::fromStdString(settings->getGradleDependenciesDirectory().str()));
	}
}

void QtProjectWizzardContentPathDependenciesGradle::save()
{
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_settings);
	if (settings)
	{
		settings->setGradleDependenciesDirectory(FilePath(m_picker->getText().toStdString()));
	}
}
