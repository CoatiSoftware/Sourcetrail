#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"

#include <QFormLayout>
#include <QMessageBox>

#include "settings/CxxProjectSettings.h"

#include "utility/logging/logging.h"

QtProjectWizzardContentData::QtProjectWizzardContentData(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_projectName(nullptr)
	, m_projectFileLocation(nullptr)
	, m_language(nullptr)
	, m_standard(nullptr)
	, m_buildFilePicker(nullptr)
{
}

void QtProjectWizzardContentData::populateWindow(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(0, 15);
	row++;

	populateForm(layout, row);

	layout->setRowMinimumHeight(row, 15);
	layout->setRowStretch(row, 1);

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 3);
}

void QtProjectWizzardContentData::populateForm(QGridLayout* layout, int& row)
{
	addNameAndLocation(layout, row);
	addLanguageAndStandard(layout, row);
}

void QtProjectWizzardContentData::load()
{
	if (m_projectName)
	{
		m_projectName->setText(QString::fromStdString(m_settings->getProjectName()));
		m_projectFileLocation->setText(QString::fromStdString(m_settings->getProjectFileLocation().str()));
	}

	if (m_language)
	{
		LanguageType type = m_settings->getLanguage();

		if (type == LANGUAGE_UNKNOWN)
		{
			LOG_ERROR("No language type defined");
			return;
		}

		m_language->setText(languageTypeToString(type).c_str());

		m_standard->clear();
		std::vector<std::string> standards = m_settings->getLanguageStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, standards[i].c_str());
		}

		m_standard->setCurrentText(QString::fromStdString(m_settings->getStandard()));
	}
}

void QtProjectWizzardContentData::save()
{
	if (m_projectName)
	{
		m_settings->setProjectName(m_projectName->text().toStdString());
		m_settings->setProjectFileLocation(m_projectFileLocation->getText().toStdString());
	}

	if (m_standard)
	{
		m_settings->setStandard(m_standard->currentText().toStdString());
	}
}

bool QtProjectWizzardContentData::check()
{
	if (!m_projectName)
	{
		return true;
	}

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
		msgBox.setText("Please define the location of the project file.");
		msgBox.exec();
		return false;
	}

	if (!FilePath(m_projectFileLocation->getText().toStdString()).expandEnvironmentVariables().exists())
	{
		QMessageBox msgBox;
		msgBox.setText("The specified location does not exist.");
		msgBox.exec();
		return false;
	}

	return true;
}

QSize QtProjectWizzardContentData::preferredWindowSize() const
{
	return QSize(580, 340);
}

void QtProjectWizzardContentData::addNameAndLocation(QGridLayout* layout, int& row)
{
	QLabel* nameLabel = createFormLabel("Name");
	m_projectName = new QLineEdit();
	m_projectName->setObjectName("name");
	m_projectName->setAttribute(Qt::WA_MacShowFocusRect, 0);

	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectName, row, QtProjectWizzardWindow::BACK_COL);
	row++;


	QLabel* locationLabel = createFormLabel("Location");
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setPickDirectory(true);

	layout->addWidget(locationLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectFileLocation, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentData::addLanguageAndStandard(QGridLayout* layout, int& row)
{
	QLabel* languageLabel = new QLabel("Language");
	languageLabel->setObjectName("label");
	m_language = new QLabel();

	layout->addWidget(languageLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_language, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;


	QLabel* standardLabel = createFormLabel("Standard");
	m_standard = new QComboBox();

	layout->addWidget(standardLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);

	row++;
}

void QtProjectWizzardContentData::addBuildFilePicker(
	QGridLayout* layout, int& row, const QString& name, const QString& filter)
{
	QLabel* label = createFormLabel(name);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_buildFilePicker = new QtLocationPicker(this);
	m_buildFilePicker->setFileFilter(filter);

	layout->addWidget(m_buildFilePicker, row, QtProjectWizzardWindow::BACK_COL);

	row++;

	QLabel* description = new QLabel(
		"The project will stay up-to-date with changes in the compilation database on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizzardWindow::BACK_COL);

	row++;
}


QtProjectWizzardContentDataCDB::QtProjectWizzardContentDataCDB(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContentData(settings, window)
{
}

void QtProjectWizzardContentDataCDB::populateForm(QGridLayout* layout, int& row)
{
	QString name = "Compilation Database";
	QString filter = "JSON Compilation Database (*.json)";

	addNameAndLocation(layout, row);

	layout->setRowMinimumHeight(row++, 20);

	addBuildFilePicker(layout, row, name, filter);
}

void QtProjectWizzardContentDataCDB::load()
{
	QtProjectWizzardContentData::load();
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		m_buildFilePicker->setText(QString::fromStdString(cxxSettings->getCompilationDatabasePath().str()));
	}
}

void QtProjectWizzardContentDataCDB::save()
{
	QtProjectWizzardContentData::save();

	FilePath path = m_buildFilePicker->getText().toStdString();
	if (!path.exists() || path.extension() != ".json")
	{
		return;
	}

	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setCompilationDatabasePath(path);
	}
}

bool QtProjectWizzardContentDataCDB::check()
{
	if (!QtProjectWizzardContentData::check())
	{
		return false;
	}

	FilePath path = m_buildFilePicker->getText().toStdString();
	if (!path.exists() || path.extension() != ".json")
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a valid compilation database file (*.json).");
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizzardContentDataCDB::refreshClicked()
{
}
