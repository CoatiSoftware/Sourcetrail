#include "qt/window/project_wizzard/QtProjectWizzardContentBuildFile.h"

#include <QMessageBox>
#include <QPushButton>

#include "qt/element/QtLocationPicker.h"

QtProjectWizzardContentBuildFile::QtProjectWizzardContentBuildFile(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
	, m_type(QtProjectWizzardContentSelect::PROJECT_EMPTY)
{
	if (!m_settings->getVisualStudioSolutionPath().empty())
	{
		m_type = QtProjectWizzardContentSelect::PROJECT_VS;
	}
	else if (!m_settings->getCompilationDatabasePath().empty())
	{
		m_type = QtProjectWizzardContentSelect::PROJECT_CDB;
	}
}

QtProjectWizzardContentSelect::ProjectType QtProjectWizzardContentBuildFile::getType() const
{
	return m_type;
}

void QtProjectWizzardContentBuildFile::populateForm(QGridLayout* layout, int& row)
{
	QString name;
	QString filter;

	switch (m_type)
	{
		case QtProjectWizzardContentSelect::PROJECT_EMPTY:
			name = "Build Text";
			filter = "Text (*.txt)";
			return;
		case QtProjectWizzardContentSelect::PROJECT_VS:
			name = "Visual Studio Solution";
			filter = "Visual Studio Solution (*.sln)";
			break;
		case QtProjectWizzardContentSelect::PROJECT_CDB:
			name = "Compilation Database";
			filter = "JSON Compilation Database (*.json)";
			break;
	}

	QLabel* label = createFormLabel(name);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_picker = new QtLocationPicker(this);
	m_picker->setFileFilter(filter);

	QPushButton* button = new QPushButton("", this);
	button->setObjectName("refreshButton");
	button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	button->setToolTip("refresh paths");
	connect(button, SIGNAL(clicked()), this, SLOT(refreshClicked()));

	if (m_type == QtProjectWizzardContentSelect::PROJECT_CDB)
	{
		button->hide();
	}

	m_picker->layout()->addWidget(button);

	layout->addWidget(m_picker, row, QtProjectWizzardWindow::BACK_COL);

	row++;
}

void QtProjectWizzardContentBuildFile::load()
{
	switch (m_type)
	{
		case QtProjectWizzardContentSelect::PROJECT_EMPTY:
			return;
		case QtProjectWizzardContentSelect::PROJECT_VS:
			m_picker->setText(QString::fromStdString(m_settings->getVisualStudioSolutionPath().str()));
			break;
		case QtProjectWizzardContentSelect::PROJECT_CDB:
			m_picker->setText(QString::fromStdString(m_settings->getCompilationDatabasePath().str()));
			break;
	}
}

void QtProjectWizzardContentBuildFile::save()
{
	switch (m_type)
	{
		case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		case QtProjectWizzardContentSelect::PROJECT_VS:
			break;
		case QtProjectWizzardContentSelect::PROJECT_CDB:
		{
			FilePath path = m_picker->getText().toStdString();
			if (!path.exists() || path.extension() != ".json")
			{
				return;
			}
			m_settings->setCompilationDatabasePath(m_picker->getText().toStdString());
			break;
		}
	}
}

bool QtProjectWizzardContentBuildFile::check()
{
	switch (m_type)
	{
		case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		case QtProjectWizzardContentSelect::PROJECT_VS:
			break;
		case QtProjectWizzardContentSelect::PROJECT_CDB:
		{
			FilePath path = m_picker->getText().toStdString();
			if (!path.exists() || path.extension() != ".json")
			{
				QMessageBox msgBox;
				msgBox.setText("Please enter a valid compilation database file (*.json).");
				msgBox.exec();
				return false;
			}
		}
	}

	return true;
}

void QtProjectWizzardContentBuildFile::refreshClicked()
{
	FilePath path = FilePath(m_picker->getText().toStdString());
	if (!path.exists())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a valid file path.");
		msgBox.exec();
		return;
	}

	QMessageBox::StandardButton reply =
		QMessageBox::question(
			this,
			"Refresh Paths",
			"Do you really want to refresh from the given file? All changes you have made to the project's analyzed "
			"paths and header search paths will be lost.",
			QMessageBox::Yes | QMessageBox::No
		);

	if (reply == QMessageBox::No)
	{
		return;
	}

	switch (m_type)
	{
		case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		case QtProjectWizzardContentSelect::PROJECT_CDB:
			break;
		case QtProjectWizzardContentSelect::PROJECT_VS:
			emit refreshVisualStudioSolution(path.str());
			break;
	}
}
