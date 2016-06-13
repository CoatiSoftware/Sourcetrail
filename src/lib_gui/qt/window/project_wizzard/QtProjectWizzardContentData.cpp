#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"

#include <QFormLayout>
#include <QMessageBox>

QtProjectWizzardContentData::QtProjectWizzardContentData(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_projectName(nullptr)
	, m_projectFileLocation(nullptr)
	, m_language(nullptr)
	, m_cppStandard(nullptr)
	, m_cStandard(nullptr)
	, m_buildFilePicker(nullptr)
{
}

void QtProjectWizzardContentData::populateWindow(QGridLayout* layout)
{
	int row = 0;
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
		if (m_settings->getLanguage().length() > 0)
		{
			m_language->setCurrentText(QString::fromStdString(m_settings->getLanguage()));
		}

		if (m_settings->getStandard().length() > 0)
		{
			if (m_language->currentIndex() == 0) // c++
			{
				m_cppStandard->setCurrentText(QString::fromStdString(m_settings->getStandard()));
			}
			else if (m_language->currentIndex() == 1) // c
			{
				m_cStandard->setCurrentText(QString::fromStdString(m_settings->getStandard()));
			}

			handleSelectionChanged(m_language->currentIndex());
		}
	}
}

void QtProjectWizzardContentData::save()
{
	if (m_projectName)
	{
		m_settings->setProjectName(m_projectName->text().toStdString());
		m_settings->setProjectFileLocation(m_projectFileLocation->getText().toStdString());
	}

	if (m_language)
	{
		m_settings->setLanguage(m_language->currentText().toStdString());

		if (m_cppStandard->isVisible())
		{
			m_settings->setStandard(m_cppStandard->currentText().toStdString());
		}
		else if (m_cStandard->isVisible())
		{
			m_settings->setStandard(m_cStandard->currentText().toStdString());
		}
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
	m_language = new QComboBox();
	m_language->insertItem(0, "C++");
	m_language->insertItem(1, "C");
	connect(m_language, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChanged(int)));

	layout->addWidget(languageLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_language, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;


	QLabel* standardLabel = createFormLabel("Standard");

	m_cppStandard = new QComboBox();
	m_cppStandard->insertItem(0, "1z");
	m_cppStandard->insertItem(1, "14");
	m_cppStandard->insertItem(2, "1y");
	m_cppStandard->insertItem(3, "11");
	m_cppStandard->insertItem(4, "0x");
	m_cppStandard->insertItem(5, "03");
	m_cppStandard->insertItem(6, "98");

	m_cStandard = new QComboBox();
	m_cStandard->insertItem(0, "1x");
	m_cStandard->insertItem(1, "11");
	m_cStandard->insertItem(2, "9x");
	m_cStandard->insertItem(3, "99");
	m_cStandard->insertItem(4, "90");
	m_cStandard->insertItem(5, "89");

	layout->addWidget(standardLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_cppStandard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	layout->addWidget(m_cStandard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizzardContentData::addBuildFilePicker(
	QGridLayout* layout, int& row, const QString& name, const QString& filter)
{
	QLabel* label = createFormLabel(name);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_buildFilePicker = new QtLocationPicker(this);
	m_buildFilePicker->setFileFilter(filter);

	// QPushButton* button = new QPushButton("", this);
	// button->setObjectName("refreshButton");
	// button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	// button->setToolTip("refresh paths");
	// connect(button, SIGNAL(clicked()), this, SLOT(refreshClicked()));

	// m_buildFilePicker->layout()->addWidget(button);

	layout->addWidget(m_buildFilePicker, row, QtProjectWizzardWindow::BACK_COL);

	row++;

	QLabel* description = new QLabel(
		"The project will stay up-to-date with changes in the compilation database on refresh.", this);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description, row, QtProjectWizzardWindow::BACK_COL);

	row++;
}

void QtProjectWizzardContentData::handleSelectionChanged(int index)
{
	if (!m_language)
	{
		return;
	}

	if (index != 0)
	{
		m_cStandard->show();
		m_cppStandard->hide();
	}
	else
	{
		m_cppStandard->show();
		m_cStandard->hide();
	}
}


QtProjectWizzardContentDataCDB::QtProjectWizzardContentDataCDB(
	ProjectSettings* settings, QtProjectWizzardWindow* window)
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

	m_buildFilePicker->setText(QString::fromStdString(m_settings->getCompilationDatabasePath().str()));
}

void QtProjectWizzardContentDataCDB::save()
{
	QtProjectWizzardContentData::save();

	FilePath path = m_buildFilePicker->getText().toStdString();
	if (!path.exists() || path.extension() != ".json")
	{
		return;
	}
	m_settings->setCompilationDatabasePath(path);
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
