#include "QtProjectWizardContentPathCodeblocksProject.h"

#include "QtProjectWizardContentPathsIndexedHeaders.h"
#include "SourceGroupCxxCodeblocks.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "utility.h"
#include "utilityFile.h"

QtProjectWizardContentPathCodeblocksProject::QtProjectWizardContentPathCodeblocksProject(
	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings), m_filePaths([&]() {
		return utility::getAsRelativeIfShorter(
			utility::toVector(SourceGroupCxxCodeblocks(m_settings).getAllSourceFilePaths()),
			m_settings->getProjectDirectoryPath());
	})
{
	setTitleString(QStringLiteral("Code::Blocks Project (.cbp)"));
	setHelpString(
		"Select the Code::Blocks file for the project. Sourcetrail will index your project based "
		"on the settings "
		"this file. It contains using all include paths and compiler flags required. The "
		"Sourcetrail project "
		"will stay up to date with changes in the Code::Blocks project on every refresh.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
	setFileEndings({L".cbp"});
	setIsRequired(true);
}

void QtProjectWizardContentPathCodeblocksProject::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter(QStringLiteral("Code::Blocks Project (*.cbp)"));
	connect(
		m_picker,
		&QtLocationPicker::locationPicked,
		this,
		&QtProjectWizardContentPathCodeblocksProject::pickedPath);

	QLabel* description = new QLabel(
		"Sourcetrail will use all settings from the Code::Blocks project and stay up-to-date with "
		"changes on refresh.",
		this);
	description->setObjectName(QStringLiteral("description"));
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizardWindow::BACK_COL);
	row++;

	QLabel* title = createFormSubLabel(QStringLiteral("Source Files to Index"));
	layout->addWidget(title, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_fileCountLabel = new QLabel(QLatin1String(""));
	m_fileCountLabel->setWordWrap(true);
	layout->addWidget(m_fileCountLabel, row, QtProjectWizardWindow::BACK_COL, Qt::AlignTop);
	row++;

	addFilesButton(QStringLiteral("show source files"), layout, row);
	row++;
}

void QtProjectWizardContentPathCodeblocksProject::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getCodeblocksProjectPath().wstr()));

	m_filePaths.clear();

	if (m_fileCountLabel)
	{
		m_fileCountLabel->setText(
			"<b>" + QString::number(getFilePaths().size()) +
			"</b> source files were found in the Code::Blocks project.");
	}
}

void QtProjectWizardContentPathCodeblocksProject::save()
{
	m_settings->setCodeblocksProjectPath(FilePath(m_picker->getText().toStdWString()));
}

std::vector<FilePath> QtProjectWizardContentPathCodeblocksProject::getFilePaths() const
{
	return m_filePaths.getValue();
}

QString QtProjectWizardContentPathCodeblocksProject::getFileNamesTitle() const
{
	return QStringLiteral("Source Files");
}

QString QtProjectWizardContentPathCodeblocksProject::getFileNamesDescription() const
{
	return QStringLiteral(" source files will be indexed.");
}

void QtProjectWizardContentPathCodeblocksProject::pickedPath()
{
	m_window->saveContent();

	const FilePath projectPath = m_settings->getProjectDirectoryPath();

	std::set<FilePath> indexedHeaderPaths;
	for (const FilePath& path:
		 QtProjectWizardContentPathsIndexedHeaders::getIndexedPathsDerivedFromCodeblocksProject(
			 m_settings))
	{
		if (projectPath.contains(path))
		{
			indexedHeaderPaths.insert(path.getRelativeTo(
				projectPath));	  // the relative path is always shorter than the  absolute path
		}
	}
	m_settings->setIndexedHeaderPaths(utility::toVector(indexedHeaderPaths));

	m_window->loadContent();
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathCodeblocksProject::getSourceGroupSettings()
{
	return m_settings;
}
