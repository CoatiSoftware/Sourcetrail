#include "QtProjectWizzardContentPath.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Application.h"
#include "ApplicationSettings.h"
#include "FileManager.h"
#include "MessageStatus.h"
#include "QtLocationPicker.h"
#include "QtDialogView.h"
#include "QtProjectWizzardContentPaths.h"
#include "ResourcePaths.h"
#include "ScopedFunctor.h"
#include "SonargraphProject.h"
#include "SourceGroupCxxCdb.h"
#include "SourceGroupCxxCodeblocks.h"
#include "SourceGroupCxxSonargraph.h"
#include "SourceGroupJavaSonargraph.h"
#include "SourceGroupJavaGradle.h"
#include "SourceGroupJavaMaven.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "SourceGroupSettingsJavaMaven.h"
#include "SourceGroupSettingsJavaGradle.h"
#include "SourceGroupSettingsJavaSonargraph.h"
#include "SourceGroupSettingsPythonEmpty.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityFile.h"
#include "utilityGradle.h"
#include "utilityMaven.h"

QtProjectWizzardContentPath::QtProjectWizzardContentPath(QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(window)
	, m_makePathRelativeToProjectFileLocation(true)
	, m_allowEmpty(false)
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
	m_picker->setPlaceholderText(m_placeholderString);

	if (m_makePathRelativeToProjectFileLocation)
	{
		m_picker->setRelativeRootDirectory(getSourceGroupSettings()->getProjectDirectoryPath());
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
			if (m_allowEmpty)
			{
				break;
			}
			else
			{
				error = "Please define a path at \"" + m_titleString + "\".";
				break;
			}
		}

		FilePath path = getSourceGroupSettings()->makePathExpandedAndAbsolute(FilePath(m_picker->getText().toStdWString()));

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

	if (!error.isEmpty())
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

void QtProjectWizzardContentPath::setPlaceholderString(const QString& placeholder)
{
	m_placeholderString = placeholder;
}

void QtProjectWizzardContentPath::setFileEndings(const std::set<std::wstring>& fileEndings)
{
	m_fileEndings = fileEndings;
}

void QtProjectWizzardContentPath::setAllowEmpty(bool allowEmpty)
{
	m_allowEmpty = allowEmpty;
}

QtProjectWizzardContentPathCDB::QtProjectWizzardContentPathCDB(
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
	, m_filePaths([&]()
		{
			return utility::getAsRelativeIfShorter(
				utility::toVector(SourceGroupCxxCdb(m_settings).getAllSourceFilePaths()),
				m_settings->getProjectDirectoryPath()
			);
		}
	)
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
	m_picker->setText(QString::fromStdWString(m_settings->getCompilationDatabasePath().wstr()));

	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText("<b>" + QString::number(getFilePaths().size()) + "</b> source files were found in the compilation database.");
	}
}

void QtProjectWizzardContentPathCDB::save()
{
	m_settings->setCompilationDatabasePath(FilePath(m_picker->getText().toStdWString()));
}

std::vector<FilePath> QtProjectWizzardContentPathCDB::getFilePaths() const
{
	return m_filePaths.getValue();
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

	const FilePath projectPath = m_settings->getProjectDirectoryPath();

	std::set<FilePath> indexedHeaderPaths;
	for (const FilePath& path : QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromCDB(m_settings))
	{
		if (projectPath.contains(path))
		{
			indexedHeaderPaths.insert(path.getRelativeTo(projectPath)); // the relative path is always shorter than the  absolute path
		}
	}
	m_settings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));

	m_window->loadContent();
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathCDB::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentCodeblocksProjectPath::QtProjectWizzardContentCodeblocksProjectPath(
	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
	, m_filePaths([&]()
		{
			return utility::getAsRelativeIfShorter(
				utility::toVector(SourceGroupCxxCodeblocks(m_settings).getAllSourceFilePaths()),
				m_settings->getProjectDirectoryPath()
			);
		}
	)
{
	setTitleString("Code::Blocks Project (.cbp)");
	setHelpString(
		"Select the Code::Blocks file for the project. Sourcetrail will index your project based on the settings "
		"this file. It contains using all include paths and compiler flags required. The Sourcetrail project "
		"will stay up to date with changes in the Code::Blocks project on every refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
	setFileEndings({ L".cbp" });
}

void QtProjectWizzardContentCodeblocksProjectPath::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Code::Blocks Project (*.cbp)");
	connect(m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizzardContentCodeblocksProjectPath::pickedPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all settings from the Code::Blocks project and stay up-to-date with changes on refresh.", this);
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

void QtProjectWizzardContentCodeblocksProjectPath::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getCodeblocksProjectPath().wstr()));

	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText("<b>" + QString::number(getFilePaths().size()) + "</b> source files were found in the Code::Blocks project.");
	}
}

void QtProjectWizzardContentCodeblocksProjectPath::save()
{
	m_settings->setCodeblocksProjectPath(FilePath(m_picker->getText().toStdWString()));
}

std::vector<FilePath> QtProjectWizzardContentCodeblocksProjectPath::getFilePaths() const
{
	return m_filePaths.getValue();
}

QString QtProjectWizzardContentCodeblocksProjectPath::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizzardContentCodeblocksProjectPath::getFileNamesDescription() const
{
	return " source files will be indexed.";
}

void QtProjectWizzardContentCodeblocksProjectPath::pickedPath()
{
	m_window->saveContent();

	const FilePath projectPath = m_settings->getProjectDirectoryPath();

	std::set<FilePath> indexedHeaderPaths;
	for (const FilePath& path : QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromCodeblocksProject(m_settings))
	{
		if (projectPath.contains(path))
		{
			indexedHeaderPaths.insert(path.getRelativeTo(projectPath)); // the relative path is always shorter than the  absolute path
		}
	}
	m_settings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));

	m_window->loadContent();
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentCodeblocksProjectPath::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentSonargraphProjectPath::QtProjectWizzardContentSonargraphProjectPath(
	std::shared_ptr<SourceGroupSettings> settings,
	std::shared_ptr<SourceGroupSettingsCxxSonargraph> settingsCxxSonargraph,
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settingsWithSonargraphProjectPath,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
	, m_settingsCxxSonargraph(settingsCxxSonargraph)
	, m_settingsWithSonargraphProjectPath(settingsWithSonargraphProjectPath)
	, m_filePaths([&]()
		{
			std::set<FilePath> allSourceFilePaths;
			if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(m_settings))
			{
				allSourceFilePaths = SourceGroupCxxSonargraph(settings).getAllSourceFilePaths();
			}
			else if (std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaSonargraph>(m_settings))
			{
				allSourceFilePaths = SourceGroupJavaSonargraph(settings).getAllSourceFilePaths();
			}

			return utility::getAsRelativeIfShorter(
				utility::toVector(allSourceFilePaths),
				m_settings->getProjectDirectoryPath()
			);
		}
	)
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
	m_picker->setText(QString::fromStdWString(m_settingsWithSonargraphProjectPath->getSonargraphProjectPath().wstr()));

	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText("<b>" + QString::number(getFilePaths().size()) + "</b> source files were found in the Sonargraph project.");
	}
}

void QtProjectWizzardContentSonargraphProjectPath::save()
{
	m_settingsWithSonargraphProjectPath->setSonargraphProjectPath(FilePath(m_picker->getText().toStdWString()));
}

bool QtProjectWizzardContentSonargraphProjectPath::check()
{
	if (!QtProjectWizzardContentPath::check())
	{
		return false;
	}

	if (std::shared_ptr<Sonargraph::Project> sonargraphProject = Sonargraph::Project::load(
		m_settingsWithSonargraphProjectPath->getSonargraphProjectPathExpandedAndAbsolute(), m_settings->getLanguage()
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
	return true;
}

std::vector<FilePath> QtProjectWizzardContentSonargraphProjectPath::getFilePaths() const
{
	return m_filePaths.getValue();
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

	if (m_settingsCxxSonargraph)
	{
		const FilePath projectPath = m_settings->getProjectDirectoryPath();

		std::set<FilePath> indexedHeaderPaths;
		for (const FilePath& path : QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromSonargraphProject(m_settingsCxxSonargraph))
		{
			if (projectPath.contains(path))
			{
				indexedHeaderPaths.insert(path.getRelativeTo(projectPath)); // the relative path is always shorter than the  absolute path
			}
		}
		m_settingsCxxSonargraph->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));
	}
	m_window->loadContent();
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentSonargraphProjectPath::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentPathSourceMaven::QtProjectWizzardContentPathSourceMaven(
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
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
	m_picker->setText(QString::fromStdWString(m_settings->getMavenProjectFilePath().wstr()));
	m_shouldIndexTests->setChecked(m_settings->getShouldIndexMavenTests());
}

void QtProjectWizzardContentPathSourceMaven::save()
{
	m_settings->setMavenProjectFilePath(FilePath(m_picker->getText().toStdWString()));
	m_settings->setShouldIndexMavenTests(m_shouldIndexTests->isChecked());
}

std::vector<FilePath> QtProjectWizzardContentPathSourceMaven::getFilePaths() const
{
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath mavenProjectRoot = m_settings->getMavenProjectFilePathExpandedAndAbsolute().getParentDirectory();

		if (!mavenProjectRoot.exists())
		{
			LOG_INFO("Could not find any source file paths because Maven project path does not exist.");
			return std::vector<FilePath>();
		}

		QtDialogView* dialogView = dynamic_cast<QtDialogView*>(
			Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP).get());

		ScopedFunctor scopedFunctor([&dialogView]() {
			dialogView->hideUnknownProgressDialog();
		});

		dialogView->setParentWindow(m_window);
		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Maven\nGenerating Source Files");

		const std::wstring errorMessage = utility::mavenGenerateSources(mavenPath, mavenProjectRoot);
		if (!errorMessage.empty())
		{
			MessageStatus(errorMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(errorMessage);
			return std::vector<FilePath>();
		}
	}

	return utility::getAsRelativeIfShorter(
		utility::toVector(SourceGroupJavaMaven(m_settings).getAllSourceFilePaths()),
		m_settings->getProjectDirectoryPath()
	);
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathSourceMaven::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentPathDependenciesMaven::QtProjectWizzardContentPathDependenciesMaven(
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
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
	m_picker->setText(QString::fromStdWString(m_settings->getMavenDependenciesDirectory().wstr()));
}

void QtProjectWizzardContentPathDependenciesMaven::save()
{
	m_settings->setMavenDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathDependenciesMaven::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentPathSourceGradle::QtProjectWizzardContentPathSourceGradle(
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
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
	m_picker->setText(QString::fromStdWString(m_settings->getGradleProjectFilePath().wstr()));
	m_shouldIndexTests->setChecked(m_settings->getShouldIndexGradleTests());
}

void QtProjectWizzardContentPathSourceGradle::save()
{
	m_settings->setGradleProjectFilePath(FilePath(m_picker->getText().toStdWString()));
	m_settings->setShouldIndexGradleTests(m_shouldIndexTests->isChecked());
}

std::vector<FilePath> QtProjectWizzardContentPathSourceGradle::getFilePaths() const
{
	QtDialogView* dialogView = dynamic_cast<QtDialogView*>(
		Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP).get());
	dialogView->setParentWindow(m_window);

	return utility::getAsRelativeIfShorter(
		utility::toVector(SourceGroupJavaGradle(m_settings).getAllSourceFilePaths()),
		m_settings->getProjectDirectoryPath()
	);
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathSourceGradle::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentPathDependenciesGradle::QtProjectWizzardContentPathDependenciesGradle(
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
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
	m_picker->setText(QString::fromStdWString(m_settings->getGradleDependenciesDirectory().wstr()));
}

void QtProjectWizzardContentPathDependenciesGradle::save()
{
	m_settings->setGradleDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathDependenciesGradle::getSourceGroupSettings()
{
	return m_settings;
}


QtProjectWizzardContentPathPythonEnvironment::QtProjectWizzardContentPathPythonEnvironment(
	std::shared_ptr<SourceGroupSettingsPythonEmpty> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPath(window)
	, m_settings(settings)
{
	setTitleString("Python Environment");
	setHelpString(
		"Here you can specify the path to the directory or to the executable of the (virtual) Python environment that should be used to resolve "
		"dependencies within the indexed source code. <br />"
		"<br />"
		"If you would run:<br />"
		"<br />"
		"$ cd C:\\dev\\python\\envs<br />"
		"$ virtualenv py37<br />"
		"<br />"
		"you would set it to \"C:\\dev\\python\\envs\\py37\" or \"C:\\dev\\python\\envs\\py37\\Scripts\\python.exe\". <br />"
		"Leave blank to use the default Python environment. You can make use of environment variables with ${ENV_VAR}."
	);
	setPlaceholderString("Use Default");
	setAllowEmpty(true);
}

void QtProjectWizzardContentPathPythonEnvironment::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPath::populate(layout, row);
	connect(
		m_picker, &QtLocationPicker::textChanged,
		this, &QtProjectWizzardContentPathPythonEnvironment::onTextChanged
	);

	m_resultLabel = new QLabel();
	m_resultLabel->setWordWrap(true);
	layout->addWidget(m_resultLabel, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentPathPythonEnvironment::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getEnvironmentPath().wstr()));
}

void QtProjectWizzardContentPathPythonEnvironment::save()
{
	m_settings->setEnvironmentPath(FilePath(m_picker->getText().toStdWString()));
}

void QtProjectWizzardContentPathPythonEnvironment::onTextChanged(const QString& text)
{
	if (text.isEmpty())
	{
		m_resultLabel->clear();
	}
	else
	{
		m_resultLabel->setText("Checking validity of Python environment...");
		std::thread([=]() {
			std::pair<int, std::string> out = utility::executeProcess(
				"\"" + ResourcePaths::getPythonPath().str() + "SourcetrailPythonIndexer\" check-environment " +
				"--environment-path \"" + m_settings->makePathExpandedAndAbsolute(FilePath(text.toStdWString())).str() + "\"",
				FilePath(),
				5000
			);
			m_onQtThread(
				[=]()
				{
					if (out.first == 0)
					{
						m_resultLabel->setText(QString::fromStdString(out.second));
					}
					else
					{
						m_resultLabel->setText("An error occurred while checking environment path. Unable to check validity.");
					}
				}
			);
		}).detach();
	}
}

std::shared_ptr<SourceGroupSettings> QtProjectWizzardContentPathPythonEnvironment::getSourceGroupSettings()
{
	return m_settings;
}
