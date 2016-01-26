#include "qt/window/QtProjectSetupScreen.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSysInfo>

#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/utility.h"

#include "settings/ProjectSettings.h"

QtTextLine::QtTextLine(QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	setLayout(layout);

	m_data = new QtLineEdit(this);
	m_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_data->setObjectName("locationField");

	m_button = new QPushButton("...");
	m_button->setObjectName("moreButton");

	layout->addWidget(m_data);
	layout->addWidget(m_button);

	connect(m_button, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
}

QString QtTextLine::getText()
{
	return m_data->text();
}

void QtTextLine::setText(QString text)
{
	m_data->setText(text);
}

void QtTextLine::clearText()
{
	m_data->clear();
}

void QtTextLine::handleButtonPress()
{
	QString file = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "");
	if (!file.isEmpty())
	{
		m_data->setText(file);
	}
}

QtProjectSetupScreen::QtProjectSetupScreen(QWidget *parent)
	: QtSettingsWindow(parent)
	, m_frameworkPaths(nullptr)
{
}

QSize QtProjectSetupScreen::sizeHint() const
{
	return QSize(600,600);
}

void QtProjectSetupScreen::clear()
{
	m_projectName->clear();
	m_projectFileLocation->clearText();

	m_language->setCurrentIndex(0);
	m_cppStandard->setCurrentIndex(0);
	m_cStandard->setCurrentIndex(0);

	m_sourcePaths->clear();
	m_includePaths->clear();

	if (m_frameworkPaths)
	{
		m_frameworkPaths->clear();
	}
}

void QtProjectSetupScreen::setup()
{
	setupForm();

	QPushButton* preferencesButton = new QPushButton("Preferences");
	preferencesButton->setObjectName("windowButton");
	connect(preferencesButton, SIGNAL(clicked()), this, SLOT(handlePreferencesButtonPress()));

	m_buttonsLayout->insertWidget(2, preferencesButton);
	m_buttonsLayout->insertStretch(3);
}

void QtProjectSetupScreen::loadEmpty()
{
	updateTitle("NEW PROJECT");
	updateDoneButton("Create");

	clear();
}

void QtProjectSetupScreen::loadProjectSettings()
{
	updateTitle("EDIT PROJECT");
	updateDoneButton("Save");

	ProjectSettings* projSettings = ProjectSettings::getInstance().get();

	m_projectName->setText(QString::fromStdString(projSettings->getFilePath().withoutExtension().fileName()));
	m_projectFileLocation->setText(QString::fromStdString(projSettings->getFilePath().parentDirectory().str()));

	if (projSettings->getLanguage().length() > 0)
	{
		m_language->setCurrentText(QString::fromStdString(projSettings->getLanguage()));
	}
	if (projSettings->getStandard().length() > 0)
	{
		if (m_language->currentIndex() == 0) // c++
		{
			m_cppStandard->setCurrentText(QString::fromStdString(projSettings->getStandard()));
		}
		else if (m_language->currentIndex() == 1) // c
		{
			m_cStandard->setCurrentText(QString::fromStdString(projSettings->getStandard()));
		}
	}

	m_sourcePaths->setList(projSettings->getSourcePaths());
	m_includePaths->setList(projSettings->getHeaderSearchPaths());

	if (m_frameworkPaths)
	{
		m_frameworkPaths->setList(projSettings->getFrameworkSearchPaths());
	}
}

void QtProjectSetupScreen::populateForm(QFormLayout* layout)
{
	int minimumWidthForSecondCol = 360;

	QLabel* nameLabel = new QLabel("Name");
	m_projectName = new QLineEdit();
	m_projectName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	m_projectName->setMinimumWidth(minimumWidthForSecondCol);
	m_projectName->setAttribute(Qt::WA_MacShowFocusRect, 0);
	layout->addRow(nameLabel, m_projectName);

	QLabel* locationLabel = new QLabel("Location");
	m_projectFileLocation = new QtTextLine(this);
	m_projectFileLocation->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(locationLabel, m_projectFileLocation);

	QLabel* languageLabel = new QLabel("Language");
	m_language = new QComboBox();
	m_language->insertItem(0, "C++");
	m_language->insertItem(1, "C");
	connect(m_language, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChanged(int)));
	layout->addRow(languageLabel, m_language);

	m_cppStandardLabel = new QLabel("Standard");

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

	QPushButton* helpButton;

	QWidget* sourcePathsWidget = createLabelWithHelpButton("Analyzed Paths", &helpButton);
	connect(helpButton, SIGNAL(clicked()), this, SLOT(handleSourcePathHelpPress()));
	m_sourcePaths = new QtDirectoryListBox(this);
	m_sourcePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(sourcePathsWidget, m_sourcePaths);

	QWidget* includePathsWidget = createLabelWithHelpButton("Header\nSearch Paths", &helpButton);
	connect(helpButton, SIGNAL(clicked()), this, SLOT(handleIncludePathHelpPress()));
	m_includePaths = new QtDirectoryListBox(this);
	m_includePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(includePathsWidget, m_includePaths);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		QWidget* frameworkPathsWidget = createLabelWithHelpButton("Framework\nSearch Paths", &helpButton);
		connect(helpButton, SIGNAL(clicked()), this, SLOT(handleFrameworkPathHelpPress()));
		m_frameworkPaths = new QtDirectoryListBox(this);
		m_frameworkPaths->setMinimumWidth(minimumWidthForSecondCol);
		layout->addRow(frameworkPathsWidget, m_frameworkPaths);
	}
}

void QtProjectSetupScreen::handleCancelButtonPress()
{
	emit canceled();
}

void QtProjectSetupScreen::handleUpdateButtonPress()
{
	if (m_projectName->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a project name.");
		msgBox.exec();
		return;
	}

	if (m_projectFileLocation->getText().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please define the location of the project file.");
		msgBox.exec();
		return;
	}

	if (!m_sourcePaths->getList().size())
	{
		QMessageBox msgBox;
		msgBox.setText("Please add at least one source path to your project.");
		msgBox.exec();
		return;
	}


	std::shared_ptr<ProjectSettings> projectSettings = ProjectSettings::getInstance();

	std::string newLanguage = m_language->currentText().toStdString();

	std::string newStandard = "";
	if (m_cppStandard->isVisible())
	{
		newStandard = m_cppStandard->currentText().toStdString();
	}
	else if (m_cStandard->isVisible())
	{
		newStandard = m_cStandard->currentText().toStdString();
	}

	std::vector<FilePath> newSourcePaths = m_sourcePaths->getList();
	std::vector<FilePath> newHeaderSearchPaths = m_includePaths->getList();
	std::vector<FilePath> newFrameworkPaths = m_frameworkPaths ? m_frameworkPaths->getList() : std::vector<FilePath>();

	bool somethingChanged = !(
		newLanguage == projectSettings->getLanguage() &&
		newStandard == projectSettings->getStandard() &&
		utility::isPermutation<FilePath>(newSourcePaths, projectSettings->getSourcePaths()) &&
		utility::isPermutation<FilePath>(newHeaderSearchPaths, projectSettings->getHeaderSearchPaths()) &&
		utility::isPermutation<FilePath>(newFrameworkPaths, projectSettings->getFrameworkSearchPaths())
	);

	projectSettings->clear();
	projectSettings->setLanguage(newLanguage);
	projectSettings->setStandard(newStandard);
	projectSettings->setSourcePaths(newSourcePaths);
	projectSettings->setHeaderSearchPaths(newHeaderSearchPaths);
	projectSettings->setFrameworkSearchPaths(newFrameworkPaths);

	std::string projectFilePath =
		m_projectFileLocation->getText().toStdString() + "/" + m_projectName->text().toStdString() + ".coatiproject";

	projectSettings->save(projectFilePath);

	if (m_title->text() == "NEW PROJECT")
	{
		MessageLoadProject(projectFilePath, false).dispatch();
	}
	else if (somethingChanged)
	{
		MessageLoadProject(projectFilePath, true).dispatch();
	}
	emit finished();
}

void QtProjectSetupScreen::handleSourcePathHelpPress()
{
	showHelpMessage(
		"Analyzed Paths define the source files and directories that will be analysed by Coati. Usually these are the source "
		"and header files of your project or a subset of them."
	);
}

void QtProjectSetupScreen::handleIncludePathHelpPress()
{
	showHelpMessage(
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analysed, but Coati needs "
		"them for correct analysis.\n\n"
		"Please note that you can define Header Search Paths for all your projects in Coati's preferences."
	);
}

void QtProjectSetupScreen::handleFrameworkPathHelpPress()
{
	showHelpMessage(
		"Framework Search Paths define where MacOS framework containers, that your project depends on, are found.\n\n"
		"Please note that you can define Framework Search Paths for all your projects in Coati's preferences."
	);
}

void QtProjectSetupScreen::handlePreferencesButtonPress()
{
	emit showPreferences();
}

void QtProjectSetupScreen::handleSelectionChanged(int index)
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
