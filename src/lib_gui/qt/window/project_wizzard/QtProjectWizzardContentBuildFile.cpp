#include "qt/window/project_wizzard/QtProjectWizzardContentBuildFile.h"

#include <QMessageBox>
#include <QPushButton>

#include "utility/ResourcePaths.h"

#include "qt/element/QtIconButton.h"
#include "qt/element/QtLocationPicker.h"
#include "settings/CxxProjectSettings.h"

QtProjectWizzardContentBuildFile::QtProjectWizzardContentBuildFile(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
	, m_type(PROJECT_UNKNOWN)
{
	m_type = m_settings->getProjectType();
}

ProjectType QtProjectWizzardContentBuildFile::getType() const
{
	return m_type;
}

void QtProjectWizzardContentBuildFile::populate(QGridLayout* layout, int& row)
{
	QString name;
	QString filter;

	switch (m_type)
	{
	case PROJECT_C_EMPTY:
	case PROJECT_CPP_EMPTY:
		name = "Build Text";
		filter = "Text (*.txt)";
		return;
	case PROJECT_CXX_VS:
	case PROJECT_CXX_CDB:
		name = "Compilation Database";
		filter = "JSON Compilation Database (*.json)";
		break;
	}

	QLabel* label = createFormLabel(name);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_picker = new QtLocationPicker(this);
	m_picker->setFileFilter(filter);
	m_picker->setRelativeRootDirectory(m_settings->getProjectFileLocation());

	layout->addWidget(m_picker, row, QtProjectWizzardWindow::BACK_COL);

	row++;
}

void QtProjectWizzardContentBuildFile::load()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		switch (m_type)
		{
		case PROJECT_C_EMPTY:
		case PROJECT_CPP_EMPTY:
			return;
		case PROJECT_CXX_VS:
		case PROJECT_CXX_CDB:
			m_picker->setText(QString::fromStdString(cxxSettings->getCompilationDatabasePath().str()));
			break;
		}
	}
}

void QtProjectWizzardContentBuildFile::save()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		switch (m_type)
		{
		case PROJECT_C_EMPTY:
		case PROJECT_CPP_EMPTY:
				break;
		case PROJECT_CXX_VS:
		case PROJECT_CXX_CDB:
			{
				FilePath path = m_picker->getText().toStdString();
				FilePath absPath = m_settings->makePathAbsolute(m_settings->expandPath(path));
				if (!absPath.exists() || absPath.extension() != ".json")
				{
					return;
				}
				cxxSettings->setCompilationDatabasePath(path);
				break;
			}
		}
	}
}

bool QtProjectWizzardContentBuildFile::check()
{
	switch (m_type)
	{
	case PROJECT_C_EMPTY:
	case PROJECT_CPP_EMPTY:
		break;
	case PROJECT_CXX_VS:
	case PROJECT_CXX_CDB:
		{
			FilePath path = m_picker->getText().toStdString();
			FilePath absPath = m_settings->makePathAbsolute(m_settings->expandPath(path));
			if (!absPath.exists() || absPath.extension() != ".json")
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
