#include "QtProjectWizardContentPathsIndexedHeaders.h"

#include <QMessageBox>

#include "CodeblocksProject.h"
#include "CompilationDatabase.h"
#include "IndexerCommandCxx.h"
#include "OrderedCache.h"
#include "QtPathListDialog.h"
#include "QtSelectPathsDialog.h"
#include "QtTextEditDialog.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "logging.h"
#include "utility.h"
#include "utilityFile.h"

std::vector<FilePath> QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromCodeblocksProject(
	std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> settings)
{
	const FilePath projectPath = settings->getProjectDirectoryPath();
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath codeblocksProjectPath = settings->getCodeblocksProjectPathExpandedAndAbsolute();
		if (!codeblocksProjectPath.empty() && codeblocksProjectPath.exists())
		{
			if (std::shared_ptr<Codeblocks::Project> codeblocksProject = Codeblocks::Project::load(
					codeblocksProjectPath))
			{
				OrderedCache<FilePath, FilePath> canonicalDirectoryPathCache(
					[](const FilePath& path) { return path.getCanonical(); });

				for (const FilePath& path: codeblocksProject->getAllSourceFilePathsCanonical(
						 settings->getSourceExtensions()))
				{
					indexedHeaderPaths.insert(
						canonicalDirectoryPathCache.getValue(path.getParentDirectory()));
				}
				utility::append(
					indexedHeaderPaths, codeblocksProject->getAllCxxHeaderSearchPathsCanonical());
			}
		}
		else
		{
			LOG_WARNING(
				"Unable to fetch indexed header paths. The provided Codeblocks project path does "
				"not exist.");
		}
	}

	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path: utility::getTopLevelPaths(indexedHeaderPaths))
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
			for (const FilePath& path: IndexerCommandCxx::getSourceFilesFromCDB(cdbPath))
			{
				indexedHeaderPaths.insert(path.getCanonical().getParentDirectory());
			}
			for (const FilePath& path: utility::CompilationDatabase(cdbPath).getAllHeaderPaths())
			{
				if (path.exists())
				{
					indexedHeaderPaths.insert(path.getCanonical());
				}
			}
		}
		else
		{
			LOG_WARNING(
				"Unable to fetch indexed header paths. The provided Compilation Database path does "
				"not exist.");
		}
	}

	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path: utility::getTopLevelPaths(indexedHeaderPaths))
	{
		if (path.exists())
		{
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}

QtProjectWizardContentPathsIndexedHeaders::QtProjectWizardContentPathsIndexedHeaders(
	std::shared_ptr<SourceGroupSettings> settings,
	QtProjectWizardWindow* window,
	const std::string& projectKindName)
	: QtProjectWizardContentPaths(
		  settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES, true)
	, m_projectKindName(projectKindName)
{
	m_showFilesString = QLatin1String("");

	setTitleString(QStringLiteral("Header Files & Directories to Index"));
	setHelpString(QString::fromStdString(
		"The provided " + m_projectKindName +
		" already specifies which source files are part of your project. But Sourcetrail still "
		"needs to know which header files to index as part of your project and which to skip. "
		"Choosing to skip indexing "
		"your system headers or external frameworks will significantly improve the overall "
		"indexing performance.<br />"
		"<br />"
		"Use this list to define which header files should be indexed by Sourcetrail. Provide a "
		"directory to recursively "
		"add all contained files.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.<br />"
		"<br />"
		"<b>Hint</b>: Just enter the root path of your project if you want Sourcetrail to index "
		"all contained headers it "
		"encounters.<br />"));
}

void QtProjectWizardContentPathsIndexedHeaders::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPaths::populate(layout, row);

	QPushButton* button = new QPushButton(QString::fromStdString("Select from " + m_projectKindName));
	button->setObjectName(QStringLiteral("windowButton"));
	connect(
		button, &QPushButton::clicked, this, &QtProjectWizardContentPathsIndexedHeaders::buttonClicked);

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
		QMessageBox msgBox(m_window);
		msgBox.setText(
			QStringLiteral("You didn't specify any Header Files & Directories to Index."));
		msgBox.setInformativeText(QString::fromStdString(
			"Sourcetrail will only index the source files listed in the " + m_projectKindName +
			" file and none of the included header files."));
		QPushButton* yesButton = msgBox.addButton(
			QStringLiteral("Continue"), QMessageBox::ButtonRole::YesRole);
		msgBox.addButton(QStringLiteral("Cancel"), QMessageBox::ButtonRole::NoRole);
		msgBox.setDefaultButton(yesButton);

		if (msgBox.exec() != 0)
		{
			return false;
		}
	}

	return QtProjectWizardContentPaths::check();
}

void QtProjectWizardContentPathsIndexedHeaders::buttonClicked()
{
	save();

	if (!m_filesDialog)
	{
		if (std::shared_ptr<SourceGroupSettingsCxxCodeblocks> codeblocksSettings =
				std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(m_settings))
		{
			const FilePath codeblocksProjectPath =
				codeblocksSettings->getCodeblocksProjectPathExpandedAndAbsolute();
			if (!codeblocksProjectPath.exists())
			{
				QMessageBox msgBox(m_window);
				msgBox.setText(
					QStringLiteral("The provided Code::Blocks project path does not exist."));
				msgBox.setDetailedText(QString::fromStdWString(codeblocksProjectPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = new QtSelectPathsDialog(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Code::Blocks project. Red paths "
				"do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.",
				m_window);
			m_filesDialog->setup();

			connect(
				m_filesDialog,
				&QtSelectPathsDialog::finished,
				this,
				&QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog);
			connect(
				m_filesDialog,
				&QtSelectPathsDialog::canceled,
				this,
				&QtProjectWizardContentPathsIndexedHeaders::closedFilesDialog);

			const FilePath projectPath = codeblocksSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog)
				->setPathsList(
					utility::convert<FilePath, FilePath>(
						getIndexedPathsDerivedFromCodeblocksProject(codeblocksSettings),
						[&](const FilePath& path) {
							return utility::getAsRelativeIfShorter(path, projectPath);
						}),
					codeblocksSettings->getIndexedHeaderPaths(),
					m_settings->getProjectDirectoryPath());
		}
		else if (
			std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings =
				std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
		{
			const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();
			if (!cdbPath.exists())
			{
				QMessageBox msgBox(m_window);
				msgBox.setText(
					QStringLiteral("The provided Compilation Database path does not exist."));
				msgBox.setDetailedText(QString::fromStdWString(cdbPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = new QtSelectPathsDialog(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Compilation Database. Red paths "
				"do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.",
				m_window);
			m_filesDialog->setup();

			connect(
				m_filesDialog,
				&QtSelectPathsDialog::finished,
				this,
				&QtProjectWizardContentPathsIndexedHeaders::savedFilesDialog);
			connect(
				m_filesDialog,
				&QtSelectPathsDialog::canceled,
				this,
				&QtProjectWizardContentPathsIndexedHeaders::closedFilesDialog);

			const FilePath projectPath = cdbSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog)
				->setPathsList(
					utility::convert<FilePath, FilePath>(
						getIndexedPathsDerivedFromCDB(cdbSettings),
						[&](const FilePath& path) {
							return utility::getAsRelativeIfShorter(path, projectPath);
						}),
					cdbSettings->getIndexedHeaderPaths(),
					m_settings->getProjectDirectoryPath());
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
	m_list->setPaths(dynamic_cast<QtSelectPathsDialog*>(m_filesDialog)->getPathsList());
	closedFilesDialog();
}
