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

void QtProjectWizzardContentBuildFile::refreshClicked()
{
	FilePath path = FilePath(m_picker->getText().toStdString());
	if (!path.exists())
	{
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
			break;
		case QtProjectWizzardContentSelect::PROJECT_VS:
		{
			emit refreshVisualStudioSolution(path.str());
			break;
		}
		case QtProjectWizzardContentSelect::PROJECT_CDB:
			break;
	}
}
