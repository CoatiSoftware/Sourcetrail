#include "qt/window/project_wizzard/QtProjectWizzardContentProjectData.h"

#include <QMessageBox>

#include "settings/ProjectSettings.h"

QtProjectWizzardContentProjectData::QtProjectWizzardContentProjectData(
	std::shared_ptr<ProjectSettings> projectSettings,
	QtProjectWizzardWindow* window,
	bool disableNameEditing
)
	: QtProjectWizzardContent(window)
	, m_projectSettings(projectSettings)
	, m_disableNameEditing(disableNameEditing)
	, m_projectName(nullptr)
	, m_projectFileLocation(nullptr)
{
}

void QtProjectWizzardContentProjectData::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	QLabel* nameLabel = createFormLabel("Sourcetrail Project Name");
	m_projectName = new QLineEdit();
	m_projectName->setObjectName("name");
	m_projectName->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_projectName->setEnabled(!m_disableNameEditing);

	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectName, row, QtProjectWizzardWindow::BACK_COL);
	layout->setRowMinimumHeight(row, 30);
	row++;

	QLabel* locationLabel = createFormLabel("Sourcetrail Project Location");
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setPickDirectory(true);
	m_projectFileLocation->setEnabled(!m_disableNameEditing);

	layout->addWidget(locationLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectFileLocation, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignTop);
	addHelpButton("The directory the Sourcetrail project file (.srctrlprj) will be saved to.", layout, row);
	layout->setRowMinimumHeight(row, 30);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentProjectData::load()
{
	m_projectName->setText(QString::fromStdString(m_projectSettings->getProjectName()));
	m_projectFileLocation->setText(QString::fromStdString(m_projectSettings->getProjectDirectoryPath().str()));
}

void QtProjectWizzardContentProjectData::save()
{
	m_projectSettings->setProjectFilePath(
		m_projectName->text().toStdString(),
		FilePath(m_projectFileLocation->getText().toStdString())
	);
}

bool QtProjectWizzardContentProjectData::check()
{
	if (m_projectName->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a project name.");
		msgBox.exec();
		return false;
	}

	if (m_projectFileLocation->getText().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please define the location for the Sourcetrail project file.");
		msgBox.exec();
		return false;
	}

	std::vector<FilePath> paths = FilePath(m_projectFileLocation->getText().toStdString()).expandEnvironmentVariables();
	if (paths.size() != 1 || !paths[0].exists())
	{
		QMessageBox msgBox;
		msgBox.setText("The specified location does not exist.");
		msgBox.exec();
		return false;
	}

	return true;
}
