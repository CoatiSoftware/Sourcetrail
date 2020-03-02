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
	setIsRequired(true);
}

void QtProjectWizardContentProjectData::populate(QGridLayout* layout, int& row)
{
	QLabel* nameLabel = createFormLabel(QStringLiteral("Sourcetrail Project Name"));
	m_projectName = new QLineEdit();
	m_projectName->setObjectName(QStringLiteral("name"));
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

	QLabel* locationLabel = createFormLabel(QStringLiteral("Sourcetrail Project Location"));
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setPickDirectory(true);
	m_projectFileLocation->setEnabled(!m_disableNameEditing);

	layout->addWidget(locationLabel, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectFileLocation, row, QtProjectWizardWindow::BACK_COL, Qt::AlignTop);
	addHelpButton(
		QStringLiteral("Sourcetrail Project Location"),
		QStringLiteral("The directory the Sourcetrail project file (.srctrlprj) will be saved to."),
		layout,
		row);
	layout->setRowMinimumHeight(row, 30);
	row++;
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
		QMessageBox msgBox(m_window);
		msgBox.setText(QStringLiteral("Please enter a project name."));
		msgBox.exec();
		return false;
	}

	if (!boost::filesystem::portable_file_name(m_projectName->text().toStdString()))
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(
			"The provided project name is not a valid file name. Please adjust the name "
			"accordingly.");
		msgBox.exec();
		return false;
	}

	if (m_projectFileLocation->getText().isEmpty())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(QStringLiteral("Please define the location for the Sourcetrail project file."));
		msgBox.exec();
		return false;
	}

	std::vector<FilePath> paths =
		FilePath(m_projectFileLocation->getText().toStdWString()).expandEnvironmentVariables();
	if (paths.size() != 1)
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(
			"The specified location seems to be invalid. Please make sure that the used "
			"environment variables are unambiguous.");
		msgBox.exec();
		return false;
	}
	else if (!paths.front().isAbsolute())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(
			"The specified location seems to be invalid. Please specify an absolute directory "
			"path.");
		msgBox.exec();
		return false;
	}
	else if (!paths.front().isValid())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(
			"The specified location seems to be invalid. Please check the characters used in the "
			"path.");
		msgBox.exec();
		return false;
	}
	else if (!paths[0].exists())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(
			QStringLiteral("The specified location does not exist. Do you want to create the directory?"));
		msgBox.addButton(QStringLiteral("Abort"), QMessageBox::ButtonRole::NoRole);
		QPushButton* createButton = msgBox.addButton(QStringLiteral("Create"), QMessageBox::ButtonRole::YesRole);
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

	QRegularExpression regex(QStringLiteral("[^A-Za-z0-9_.-]"));
	text.replace(regex, QStringLiteral("_"));

	m_projectName->setText(text);
	m_projectName->setCursorPosition(cursorPosition);
}
