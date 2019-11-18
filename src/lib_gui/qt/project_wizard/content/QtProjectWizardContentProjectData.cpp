#include "QtProjectWizardContentProjectData.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <boost/filesystem/path.hpp>

#include "FileSystem.h"
#include "ProjectSettings.h"

QtProjectWizardContentProjectData::QtProjectWizardContentProjectData(
	std::shared_ptr<ProjectSettings> projectSettings,
	QtProjectWizardWindow* window,
	bool disableNameEditing)
	: QtProjectWizardContent(window)
	, m_projectSettings(projectSettings)
	, m_disableNameEditing(disableNameEditing)
	, m_projectName(nullptr)
	, m_projectFileLocation(nullptr)
{
}

void QtProjectWizardContentProjectData::populate(QGridLayout* layout, int& row)
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
	connect(
		m_projectName,
		&QLineEdit::textEdited,
		this,
		&QtProjectWizardContentProjectData::onProjectNameEdited);

	layout->addWidget(nameLabel, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectName, row, QtProjectWizardWindow::BACK_COL);
	layout->setRowMinimumHeight(row, 30);
	row++;

	QLabel* locationLabel = createFormLabel("Sourcetrail Project Location");
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setPickDirectory(true);
	m_projectFileLocation->setEnabled(!m_disableNameEditing);

	layout->addWidget(locationLabel, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectFileLocation, row, QtProjectWizardWindow::BACK_COL, Qt::AlignTop);
	addHelpButton(
		"Sourcetrail Project Location",
		"The directory the Sourcetrail project file (.srctrlprj) will be saved to.",
		layout,
		row);
	layout->setRowMinimumHeight(row, 30);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizardContentProjectData::load()
{
	m_projectName->setText(QString::fromStdWString(m_projectSettings->getProjectName()));
	m_projectFileLocation->setText(
		QString::fromStdWString(m_projectSettings->getProjectDirectoryPath().wstr()));
}

void QtProjectWizardContentProjectData::save()
{
	m_projectSettings->setProjectFilePath(
		m_projectName->text().toStdWString(),
		FilePath(m_projectFileLocation->getText().toStdWString()));
}

bool QtProjectWizardContentProjectData::check()
{
	if (m_projectName->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a project name.");
		msgBox.exec();
		return false;
	}

	if (!boost::filesystem::portable_file_name(m_projectName->text().toStdString()))
	{
		QMessageBox msgBox;
		msgBox.setText(
			"The provided project name is not a valid file name. Please adjust the name "
			"accordingly.");
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

	std::vector<FilePath> paths =
		FilePath(m_projectFileLocation->getText().toStdWString()).expandEnvironmentVariables();
	if (paths.size() != 1 || !paths[0].isAbsolute())
	{
		QMessageBox msgBox;
		msgBox.setText(
			"The specified location is invalid. Please enter an absolute directory path.");
		msgBox.exec();
		return false;
	}
	else if (!paths[0].exists())
	{
		QMessageBox msgBox;
		msgBox.setText(
			"The specified location does not exist. Do you want to create the directory?");
		msgBox.addButton("Abort", QMessageBox::ButtonRole::NoRole);
		QPushButton* createButton = msgBox.addButton("Create", QMessageBox::ButtonRole::YesRole);
		msgBox.setDefaultButton(createButton);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();
		if (ret == 1)	 // QMessageBox::Yes
		{
			FileSystem::createDirectory(paths[0]);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void QtProjectWizardContentProjectData::onProjectNameEdited(QString text)
{
	const int cursorPosition = m_projectName->cursorPosition();

	QRegularExpression regex("[^A-Za-z0-9_.-]");
	text.replace(regex, "_");

	m_projectName->setText(text);
	m_projectName->setCursorPosition(cursorPosition);
}
