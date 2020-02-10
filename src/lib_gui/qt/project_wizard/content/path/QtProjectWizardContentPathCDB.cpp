#include "QtProjectWizardContentPathCDB.h"

#include "QtProjectWizardContentPathsIndexedHeaders.h"
#include "SourceGroupCxxCdb.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "utility.h"
#include "utilityFile.h"
#include "utilitySourceGroupCxx.h"

QtProjectWizardContentPathCDB::QtProjectWizardContentPathCDB(
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings), m_filePaths([&]() {
		return utility::getAsRelativeIfShorter(
			utility::toVector(SourceGroupCxxCdb(m_settings).getAllSourceFilePaths()),
			m_settings->getProjectDirectoryPath());
	})
{
	setTitleString("Compilation Database (compile_commands.json)");
	setHelpString(
		"Select the compilation database file for the project. Sourcetrail will index your project "
		"based on the compile "
		"commands. This file contains using all include paths and compiler flags of these compile "
		"commands. The project "
		"will stay up to date with changes in the compilation database on every refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
	setFileEndings({L".json"});
	setIsRequired(true);
}

void QtProjectWizardContentPathCDB::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("JSON Compilation Database (*.json)");
	connect(
		m_picker, &QtLocationPicker::locationPicked, this, &QtProjectWizardContentPathCDB::pickedPath);
	connect(
		m_picker,
		&QtLocationPicker::textChanged,
		this,
		&QtProjectWizardContentPathCDB::onPickerTextChanged);

	QLabel* description = new QLabel(
		"Sourcetrail will use all include paths and compiler flags from the Compilation Database "
		"and stay up-to-date "
		"with changes on refresh.",
		this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizardWindow::BACK_COL);
	row++;

	QLabel* title = createFormSubLabel("Source Files to Index");
	layout->addWidget(title, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_fileCountLabel = new QLabel("");
	m_fileCountLabel->setWordWrap(true);
	layout->addWidget(m_fileCountLabel, row, QtProjectWizardWindow::BACK_COL, Qt::AlignTop);
	row++;

	addFilesButton("show source files", layout, row);
	row++;
}

void QtProjectWizardContentPathCDB::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getCompilationDatabasePath().wstr()));

	refresh();
}

void QtProjectWizardContentPathCDB::save()
{
	m_settings->setCompilationDatabasePath(FilePath(m_picker->getText().toStdWString()));
}

void QtProjectWizardContentPathCDB::refresh()
{
	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText(
			"<b>" + QString::number(getFilePaths().size()) +
			"</b> source files were found in the compilation database.");
	}
}

std::vector<FilePath> QtProjectWizardContentPathCDB::getFilePaths() const
{
	return m_filePaths.getValue();
}

QString QtProjectWizardContentPathCDB::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizardContentPathCDB::getFileNamesDescription() const
{
	return " source files will be indexed.";
}

void QtProjectWizardContentPathCDB::pickedPath()
{
	m_window->saveContent();

	const FilePath projectPath = m_settings->getProjectDirectoryPath();

	std::set<FilePath> indexedHeaderPaths;
	for (const FilePath& path:
		 QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromCDB(m_settings))
	{
		if (projectPath.contains(path))
		{
			// the relative path is always shorter than the absolute path
			indexedHeaderPaths.insert(path.getRelativeTo(projectPath));
		}
	}
	m_settings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));

	m_window->loadContent();
}

void QtProjectWizardContentPathCDB::onPickerTextChanged(const QString& text)
{
	const FilePath cdbPath = utility::getExpandedAndAbsolutePath(
		FilePath(text.toStdWString()), m_settings->getProjectDirectoryPath());
	if (!cdbPath.empty() && cdbPath.exists() &&
		cdbPath != m_settings->getCompilationDatabasePathExpandedAndAbsolute())
	{
		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = utility::loadCDB(
			cdbPath, &error);
		if (cdb && error.empty())
		{
			pickedPath();
		}
	}
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathCDB::getSourceGroupSettings()
{
	return m_settings;
}
