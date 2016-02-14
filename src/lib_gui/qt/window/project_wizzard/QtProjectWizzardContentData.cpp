#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"

#include <QFormLayout>
#include <QMessageBox>

QtProjectWizzardContentData::QtProjectWizzardContentData(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentData::populateWindow(QWidget* widget)
{
	QFormLayout* layout = new QFormLayout();
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setHorizontalSpacing(20);

	populateForm(layout);

	widget->setLayout(layout);
}

void QtProjectWizzardContentData::populateForm(QFormLayout* layout)
{
	int minimumWidthForSecondCol = 360;

	QLabel* nameLabel = new QLabel("Name");
	nameLabel->setObjectName("label");
	m_projectName = new QLineEdit();
	m_projectName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	m_projectName->setMinimumWidth(minimumWidthForSecondCol);
	m_projectName->setAttribute(Qt::WA_MacShowFocusRect, 0);
	layout->addRow(nameLabel, m_projectName);

	QLabel* locationLabel = new QLabel("Location");
	locationLabel->setObjectName("label");
	m_projectFileLocation = new QtLocationPicker(this);
	m_projectFileLocation->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(locationLabel, m_projectFileLocation);

	QLabel* languageLabel = new QLabel("Language");
	languageLabel->setObjectName("label");
	m_language = new QComboBox();
	m_language->insertItem(0, "C++");
	m_language->insertItem(1, "C");
	connect(m_language, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChanged(int)));
	layout->addRow(languageLabel, m_language);

	m_cppStandardLabel = new QLabel("Standard");
	m_cppStandardLabel->setObjectName("label");

	m_cppStandard = new QComboBox();
	m_cppStandard->insertItem(0, "1z");
	m_cppStandard->insertItem(1, "14");
	m_cppStandard->insertItem(2, "1y");
	m_cppStandard->insertItem(3, "11");
	m_cppStandard->insertItem(4, "0x");
	m_cppStandard->insertItem(5, "03");
	m_cppStandard->insertItem(6, "98");
	layout->addRow(m_cppStandardLabel, m_cppStandard);


	m_cStandardLabel = new QLabel("Standard");
	m_cStandardLabel->setObjectName("label");

	m_cStandard = new QComboBox();
	m_cStandard->insertItem(0, "1x");
	m_cStandard->insertItem(1, "11");
	m_cStandard->insertItem(2, "9x");
	m_cStandard->insertItem(3, "99");
	m_cStandard->insertItem(4, "90");
	m_cStandard->insertItem(5, "89");
	layout->addRow(m_cStandardLabel, m_cStandard);
	m_cStandardLabel->hide();
	m_cStandard->hide();
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

	return true;
}

void QtProjectWizzardContentData::handleSelectionChanged(int index)
{
	if (index != 0)
	{
		m_cStandardLabel->show();
		m_cStandard->show();
		m_cppStandardLabel->hide();
		m_cppStandard->hide();
	}
	else
	{
		m_cppStandardLabel->show();
		m_cppStandard->show();
		m_cStandardLabel->hide();
		m_cStandard->hide();
	}
}
