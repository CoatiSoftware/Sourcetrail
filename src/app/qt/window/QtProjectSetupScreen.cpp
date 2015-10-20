#include "qt/window/QtProjectSetupScreen.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSysInfo>

#include "utility/messaging/type/MessageLoadProject.h"

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
	raise();
}

QSize QtProjectSetupScreen::sizeHint() const
{
	return QSize(600,600);
}

void QtProjectSetupScreen::setup()
{
	setupForm();
}

void QtProjectSetupScreen::loadEmpty()
{
	updateTitle("NEW PROJECT");
	updateDoneButton("Create");
}

void QtProjectSetupScreen::loadProjectSettings()
{
	updateTitle("EDIT PROJECT");
	updateDoneButton("Save");

	ProjectSettings* projSettings = ProjectSettings::getInstance().get();

	m_projectName->setText(QString::fromStdString(projSettings->getFilePath().withoutExtension().fileName()));
	m_projectFileLocation->setText(QString::fromStdString(projSettings->getFilePath().parentDirectory().str()));

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
	QComboBox* language = new QComboBox();
	language->insertItem(0, "C++");
	layout->addRow(languageLabel, language);

	QLabel* sourcePathsLabel = new QLabel("Source Paths");
	m_sourcePaths = new QtDirectoryListBox(this);
	m_sourcePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(sourcePathsLabel, m_sourcePaths);

	QLabel* includePathsLabel = new QLabel("Include Paths");
	m_includePaths = new QtDirectoryListBox(this);
	m_includePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(includePathsLabel, m_includePaths);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		QLabel* frameworkPathsLabel = new QLabel("Framework Paths");
		m_frameworkPaths = new QtDirectoryListBox(this);
		m_frameworkPaths->setMinimumWidth(minimumWidthForSecondCol);
		layout->addRow(frameworkPathsLabel, m_frameworkPaths);
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

	ProjectSettings* projSettings = ProjectSettings::getInstance().get();

	projSettings->clear();

	projSettings->setSourcePaths(m_sourcePaths->getList());
	projSettings->setHeaderSearchPaths(m_includePaths->getList());

	if (m_frameworkPaths)
	{
		projSettings->setFrameworkSearchPaths(m_frameworkPaths->getList());
	}

	std::string projectFile =
		m_projectFileLocation->getText().toStdString() + "/" + m_projectName->text().toStdString() + ".xml";
	projSettings->save(projectFile);

	MessageLoadProject(projectFile).dispatch();
	emit finished();
}
