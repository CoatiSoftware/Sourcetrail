#include "QtProjectWizzardContentCustomCommand.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <boost/filesystem/path.hpp>

#include "FileSystem.h"
#include "ProjectSettings.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SqliteIndexStorage.h"

QtProjectWizzardContentCustomCommand::QtProjectWizzardContentCustomCommand(
	std::shared_ptr<SourceGroupSettingsCustomCommand> settings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_customCommand(nullptr)
{
}

void QtProjectWizzardContentCustomCommand::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	QLabel* nameLabel = createFormLabel("Custom Command");
	addHelpButton(
		"Custom Command",
		"<p>Specify the commandline call that will be executed for each source file in this Source Group. "
		"You can use the following variables, %{SOURCE_FILE_PATH} is mandatory.</p>"
		"<ul>"
			"<li><b>%{SOURCE_FILE_PATH}</b> - Path to each source file (mandatory)</li>"
			"<li><b>%{DATABASE_FILE_PATH}</b> - Path to database file: \"" +
				QString::fromStdWString(m_settings->getProjectSettings()->getTempDBFilePath().wstr()) + "\"</li>"
			"<li><b>%{DATABASE_VERSION}</b> - Database version used by this Sourcetrail version: \"" +
				QString::number(SqliteIndexStorage::getStorageVersion()) + "\"</li>"
			"<li><b>%{PROJECT_FILE_PATH}</b> - Path to project file: \"" +
				QString::fromStdWString(m_settings->getProjectSettings()->getProjectFilePath().wstr()) + "\"</li>"
		"</ul>",
		layout, row
	);

	m_customCommand = new QLineEdit();
	m_customCommand->setObjectName("name");
	m_customCommand->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_runInParallel = new QCheckBox("Run in Parallel");

	layout->setRowMinimumHeight(row, 30);

	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_customCommand, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	layout->addWidget(m_runInParallel, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentCustomCommand::load()
{
	m_customCommand->setText(QString::fromStdWString(m_settings->getCustomCommand()));
	m_runInParallel->setChecked(m_settings->getRunInParallel());
}

void QtProjectWizzardContentCustomCommand::save()
{
	m_settings->setCustomCommand(m_customCommand->text().toStdWString());
	m_settings->setRunInParallel(m_runInParallel->isChecked());
}

bool QtProjectWizzardContentCustomCommand::check()
{
	if (m_customCommand->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a custom command.");
		msgBox.exec();
		return false;
	}

	if (m_customCommand->text().toStdWString().find(L"%{SOURCE_FILE_PATH}") == std::wstring::npos)
	{
		QMessageBox msgBox;
		msgBox.setText("The variable %{SOURCE_FILE_PATH} is missing in the custom command.");
		msgBox.exec();
		return false;
	}

	return true;
}
