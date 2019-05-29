#include "QtProjectWizardContentPathSonargraphProject.h"

#include <QMessageBox>

#include "QtProjectWizardContentPathsIndexedHeaders.h"
#include "SonargraphProject.h"
#include "SourceGroupCxxSonargraph.h"
#include "SourceGroupJavaSonargraph.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "SourceGroupSettingsJavaSonargraph.h"
#include "SourceGroupSettingsWithSonargraphProjectPath.h"
#include "utility.h"
#include "utilityFile.h"

QtProjectWizardContentPathSonargraphProject::QtProjectWizardContentPathSonargraphProject(
	std::shared_ptr<SourceGroupSettings> settings,
	std::shared_ptr<SourceGroupSettingsCxxSonargraph> settingsCxxSonargraph,
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settingsWithSonargraphProjectPath,
	QtProjectWizardWindow* window
)
	: QtProjectWizardContentPath(window)
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

void QtProjectWizardContentPathSonargraphProject::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Sonargraph Project (system.sonargraph)");
	connect(m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizardContentPathSonargraphProject::pickedPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all settings from the Sonargraph project and stay up-to-date with changes on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizardWindow::BACK_COL);
	row++;

	QLabel* title = createFormLabel("Source Files to Index");
	layout->addWidget(title, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_fileCountLabel = new QLabel("");
	m_fileCountLabel->setWordWrap(true);
	layout->addWidget(m_fileCountLabel, row, QtProjectWizardWindow::BACK_COL, Qt::AlignTop);
	row++;

	addFilesButton("show source files", layout, row);
	row++;
}

void QtProjectWizardContentPathSonargraphProject::load()
{
	m_picker->setText(QString::fromStdWString(m_settingsWithSonargraphProjectPath->getSonargraphProjectPath().wstr()));

	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText("<b>" + QString::number(getFilePaths().size()) + "</b> source files were found in the Sonargraph project.");
	}
}

void QtProjectWizardContentPathSonargraphProject::save()
{
	m_settingsWithSonargraphProjectPath->setSonargraphProjectPath(FilePath(m_picker->getText().toStdWString()));
}

bool QtProjectWizardContentPathSonargraphProject::check()
{
	if (!QtProjectWizardContentPath::check())
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

std::vector<FilePath> QtProjectWizardContentPathSonargraphProject::getFilePaths() const
{
	return m_filePaths.getValue();
}

QString QtProjectWizardContentPathSonargraphProject::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizardContentPathSonargraphProject::getFileNamesDescription() const
{
	return " source files will be indexed.";
}

void QtProjectWizardContentPathSonargraphProject::pickedPath()
{
	m_window->saveContent();

	if (m_settingsCxxSonargraph)
	{
		const FilePath projectPath = m_settings->getProjectDirectoryPath();

		std::set<FilePath> indexedHeaderPaths;
		for (const FilePath& path : QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromSonargraphProject(m_settingsCxxSonargraph))
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

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathSonargraphProject::getSourceGroupSettings()
{
	return m_settings;
}
