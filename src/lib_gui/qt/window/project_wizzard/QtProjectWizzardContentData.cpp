#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"

#include <QFormLayout>
#include <QMessageBox>

QtProjectWizzardContentData::QtProjectWizzardContentData(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
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
	int minimumWidthForSecondCol = 360;

	QLabel* nameLabel = createFormLabel("Name");
	m_projectName = new QLineEdit();
	m_projectName->setObjectName("name");
	m_projectName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	m_projectName->setMinimumWidth(minimumWidthForSecondCol);
	m_projectName->setAttribute(Qt::WA_MacShowFocusRect, 0);

	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectName, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	QLabel* locationLabel = createFormLabel("Location");
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setPickDirectory(true);
	m_projectFileLocation->setMinimumWidth(minimumWidthForSecondCol);

	layout->addWidget(locationLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_projectFileLocation, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	QLabel* languageLabel = new QLabel("Language");
	languageLabel->setObjectName("label");
	m_language = new QComboBox();
	m_language->insertItem(0, "C++");
	m_language->insertItem(1, "C");
	connect(m_language, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChanged(int)));

	layout->addWidget(languageLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_language, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;


	QLabel* cppStandardLabel = createFormLabel("Standard");

	m_cppStandard = new QComboBox();
	m_cppStandard->insertItem(0, "1z");
	m_cppStandard->insertItem(1, "14");
	m_cppStandard->insertItem(2, "1y");
	m_cppStandard->insertItem(3, "11");
	m_cppStandard->insertItem(4, "0x");
	m_cppStandard->insertItem(5, "03");
	m_cppStandard->insertItem(6, "98");

	layout->addWidget(cppStandardLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_cppStandard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);


	QLabel* cStandardLabel = createFormLabel("Standard");

	m_cStandard = new QComboBox();
	m_cStandard->insertItem(0, "1x");
	m_cStandard->insertItem(1, "11");
	m_cStandard->insertItem(2, "9x");
	m_cStandard->insertItem(3, "99");
	m_cStandard->insertItem(4, "90");
	m_cStandard->insertItem(5, "89");

	layout->addWidget(cStandardLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_cStandard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);

	row++;
}

void QtProjectWizzardContentData::load()
{
	m_projectName->setText(QString::fromStdString(m_settings->getProjectName()));
	m_projectFileLocation->setText(QString::fromStdString(m_settings->getProjectFileLocation()));

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
	}
}

void QtProjectWizzardContentData::save()
{
	m_settings->setProjectName(m_projectName->text().toStdString());
	m_settings->setProjectFileLocation(m_projectFileLocation->getText().toStdString());
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

bool QtProjectWizzardContentData::check()
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
		msgBox.setText("Please define the location of the project file.");
		msgBox.exec();
		return false;
	}

	if (!FilePath(m_projectFileLocation->getText().toStdString()).exists())
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

void QtProjectWizzardContentData::handleSelectionChanged(int index)
{
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
