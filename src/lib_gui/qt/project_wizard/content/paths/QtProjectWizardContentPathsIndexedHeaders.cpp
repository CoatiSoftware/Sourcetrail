#include "QtProjectWizardContentPathsIndexedHeaders.h"

#include <QMessageBox>

#include "CodeblocksProject.h"
#include "CompilationDatabase.h"
#include "IndexerCommandCxx.h"
#include "logging.h"
#include "OrderedCache.h"
#include "QtPathListDialog.h"
#include "QtSelectPathsDialog.h"
#include "QtTextEditDialog.h"
#include "SonargraphProject.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "utility.h"
#include "utilityFile.h"

std::vector<FilePath> QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromSonargraphProject(
	std::shared_ptr<const SourceGroupSettingsCxxSonargraph> settings)
{
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath sonargraphProjectPath = settings->getSonargraphProjectPathExpandedAndAbsolute();
		if (!sonargraphProjectPath.empty() && sonargraphProjectPath.exists())
		{
			if (std::shared_ptr<Sonargraph::Project> sonargraphProject = Sonargraph::Project::load(
				sonargraphProjectPath, settings->getLanguage()
			))
			{
				for (const FilePath& path : sonargraphProject->getAllSourceFilePathsCanonical())
				{
					indexedHeaderPaths.insert(path.getCanonical().getParentDirectory());
				}
				utility::append(indexedHeaderPaths, sonargraphProject->getAllCxxHeaderSearchPathsCanonical());
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Sonargraph project path does not exist.");
		}
	}

	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path : utility::getTopLevelPaths(indexedHeaderPaths))
	{
		if (path.exists())
		{
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}

std::vector<FilePath> QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromCodeblocksProject(
	std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> settings)
{
	const FilePath projectPath = settings->getProjectDirectoryPath();
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath codeblocksProjectPath = settings->getCodeblocksProjectPathExpandedAndAbsolute();
		if (!codeblocksProjectPath.empty() && codeblocksProjectPath.exists())
		{
			if (std::shared_ptr<Codeblocks::Project> codeblocksProject = Codeblocks::Project::load(codeblocksProjectPath))
			{
				OrderedCache<FilePath, FilePath> canonicalDirectoryPathCache([](const FilePath& path) {
					return path.getCanonical();
				});

				for (const FilePath& path : codeblocksProject->getAllSourceFilePathsCanonical(settings->getSourceExtensions()))
				{
					indexedHeaderPaths.insert(canonicalDirectoryPathCache.getValue(path.getParentDirectory()));
				}
				utility::append(indexedHeaderPaths, codeblocksProject->getAllCxxHeaderSearchPathsCanonical());
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Sonargraph project path does not exist.");
		}
	}

	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path : utility::getTopLevelPaths(indexedHeaderPaths))
	{
		if (path.exists())
		{
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}

std::vector<FilePath> QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromCDB(
	std::shared_ptr<const SourceGroupSettingsCxxCdb> settings)
{
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath cdbPath = settings->getCompilationDatabasePathExpandedAndAbsolute();
		if (!cdbPath.empty() && cdbPath.exists())
		{
			for (const FilePath& path : IndexerCommandCxx::getSourceFilesFromCDB(cdbPath))
			{
				indexedHeaderPaths.insert(path.getCanonical().getParentDirectory());
			}
			for (const FilePath& path : utility::CompilationDatabase(cdbPath).getAllHeaderPaths())
			{
				if (path.exists())
				{
					indexedHeaderPaths.insert(path.getCanonical());
				}
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Compilation Database path does not exist.");
		}
	}

	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path : utility::getTopLevelPaths(indexedHeaderPaths))
	{
		if (path.exists())
		{
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}

QtProjectWizardContentPathsIndexedHeaders::QtProjectWizardContentPathsIndexedHeaders(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window, std::string projectKindName
)
	: QtProjectWizardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES)
	, m_projectKindName(projectKindName)
{
	m_showFilesString = "";

	setTitleString("Header Files & Directories to Index");
	setHelpString(QString::fromStdString(
		"The provided " + m_projectKindName + " already specifies which source files are part of your project. But Sourcetrail still "
		"needs to know which header files to index as part of your project and which to skip. Choosing to skip indexing "
		"your system headers or external frameworks will significantly improve the overall indexing performance.<br />"
		"<br />"
		"Use this list to define which header files should be indexed by Sourcetrail. Provide a directory to recursively "
		"add all contained files.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.<br />"
		"<br />"
		"<b>Hint</b>: Just enter the root path of your project if you want Sourcetrail to index all contained headers it "
		"encounters.<br />"
	));
}

void QtProjectWizardContentPathsIndexedHeaders::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPaths::populate(layout, row);

	QPushButton* button = new QPushButton(QString::fromStdString("Select from " + m_projectKindName));
	button->setObjectName("windowButton");
	connect(button, &QPushButton::clicked, this, &QtProjectWizardContentPathsIndexedHeaders::buttonClicked);

	layout->addWidget(button, row, QtProjectWizardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	row++;
}

void QtProjectWizardContentPathsIndexedHeaders::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithIndexedHeaderPaths> cdbSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithIndexedHeaderPaths>(m_settings))
	{
		m_list->setPaths(cdbSettings->getIndexedHeaderPaths());
	}
}

void QtProjectWizardContentPathsIndexedHeaders::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithIndexedHeaderPaths> cdbSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithIndexedHeaderPaths>(m_settings))
	{
		cdbSettings->setIndexedHeaderPaths(m_list->getPathsAsDisplayed());
	}
}

bool QtProjectWizardContentPathsIndexedHeaders::check()
{
	if (m_list->getPathsAsDisplayed().empty())
	{
		QMessageBox msgBox;
		msgBox.setText("You didn't specify any Header Files & Directories to Index.");
		msgBox.setInformativeText(QString::fromStdString(
			"Sourcetrail will only index the source files listed in the " + m_projectKindName +
			" file and none of the included header files."
		));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		return msgBox.exec() == QMessageBox::Ok;
	}
	else
	{
		return QtProjectWizardContentPaths::check();
	}
}

void QtProjectWizardContentPathsIndexedHeaders::buttonClicked()
{
	save();

	if (!m_filesDialog)
	{
		if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> sonargraphSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(m_settings))
		{
			const FilePath sonargraphProjectPath = sonargraphSettings->getSonargraphProjectPathExpandedAndAbsolute();
			if (!sonargraphProjectPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Sonargraph project path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(sonargraphProjectPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Sonargraph project. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizardContentPathsIndexedHeaders::closedFilesDialog);

			const FilePath projectPath = sonargraphSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromSonargraphProject(sonargraphSettings),
					[&](const FilePath& path) { return utility::getAsRelativeIfShorter(path, projectPath); }
				),
				sonargraphSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
		else if (std::shared_ptr<SourceGroupSettingsCxxCodeblocks> codeblocksSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(m_settings))
		{
			const FilePath codeblocksProjectPath = codeblocksSettings->getCodeblocksProjectPathExpandedAndAbsolute();
			if (!codeblocksProjectPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Code::Blocks project path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(codeblocksProjectPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Code::Blocks project. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizardContentPathsIndexedHeaders::closedFilesDialog);

			const FilePath projectPath = codeblocksSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromCodeblocksProject(codeblocksSettings),
					[&](const FilePath& path) { return utility::getAsRelativeIfShorter(path, projectPath); }
				),
				codeblocksSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
		else if (std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
		{
			const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();
			if (!cdbPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Compilation Database path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(cdbPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Compilation Database. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizardContentPathsIndexedHeaders::closedFilesDialog);

			const FilePath projectPath = cdbSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromCDB(cdbSettings),
					[&](const FilePath& path) { return utility::getAsRelativeIfShorter(path, projectPath); }
				),
				cdbSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
	}

	if (m_filesDialog)
	{
		m_filesDialog->showWindow();
		m_filesDialog->raise();
	}
}

void QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog()
{
	m_list->setPaths(dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->getPathsList());
	closedFilesDialog();
}
