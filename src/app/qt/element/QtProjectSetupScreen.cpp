#include "qt/element/QtProjectSetupScreen.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>

#include "settings/ProjectSettings.h"
#include "utility/logging/logging.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "qt/utility/utilityQt.h"

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
	m_data->setReadOnly(true);

	m_button = new QPushButton("...");
	m_button->setObjectName("moreButton");

	layout->addWidget(m_data);
	layout->addWidget(m_button);

	connect(m_button, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
	connect(m_data, SIGNAL(focus()), this, SLOT(handleButtonPress()));
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
{
	raise();
}

void QtProjectSetupScreen::setup()
{
	QtDeviceScaledPixmap coati_logo("data/gui/startscreen/logo.png");
	coati_logo.scaleToWidth(400);
	QLabel* coatiLogoLabel = new QLabel(m_window);
	coatiLogoLabel->setPixmap(coati_logo.pixmap());
	coatiLogoLabel->resize(coati_logo.width(), coati_logo.height());
	coatiLogoLabel->move(100, 100);

	setStyleSheet(utility::getStyleSheet("data/gui/settingwindows/projectsetup.css").c_str());
	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(25, 30, 25, 20);

	m_title = new QLabel();
	m_title->setObjectName("titleLabel");
	windowLayout->addWidget(m_title);
	windowLayout->addSpacing(30);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setObjectName("formArea");
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setWidgetResizable(true);

	QWidget* form = new QWidget();
	form->setObjectName("form");
	scrollArea->setWidget(form);

	QFormLayout *layout = new QFormLayout();
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setHorizontalSpacing(20);

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

	form->setLayout(layout);

	m_createButton = new QPushButton("Create");
	m_createButton->setObjectName("windowButton");
	m_cancelButton = new QPushButton("Cancel");
	m_cancelButton->setObjectName("windowButton");
	m_updateButton = new QPushButton("Update");
	m_updateButton->setObjectName("windowButton");

	connect(m_createButton, SIGNAL(clicked()), this, SLOT(handleCreateButtonPress()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(handleCancelButtonPress()));
	connect(m_updateButton, SIGNAL(clicked()), this, SLOT(handleUpdateButtonPress()));

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(m_cancelButton);
	buttons->addStretch();
	buttons->addWidget(m_createButton);
	buttons->addWidget(m_updateButton);

	windowLayout->addWidget(scrollArea);
	windowLayout->addSpacing(20);
	windowLayout->addLayout(buttons);

	m_window->setLayout(windowLayout);
	resize(QSize(600, 620));

	scrollArea->raise();
}

void QtProjectSetupScreen::loadEmpty()
{
	m_updateButton->hide();
	m_createButton->show();

	m_title->setText("NEW PROJECT");
}

void QtProjectSetupScreen::loadProjectSettings()
{
	m_updateButton->show();
	m_createButton->hide();

	m_title->setText("EDIT PROJECT");

	ProjectSettings* projSettings = ProjectSettings::getInstance().get();

	m_projectName->setText(QString::fromStdString(projSettings->getFilePath().withoutExtension().fileName()));
	m_projectFileLocation->setText(QString::fromStdString(projSettings->getFilePath().parentDirectory().str()));

	m_sourcePaths->setList(projSettings->getSourcePaths());
	m_includePaths->setList(projSettings->getHeaderSearchPaths());
	m_frameworkPaths->setList(projSettings->getFrameworkSearchPaths());
}

void QtProjectSetupScreen::handleCreateButtonPress()
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

void QtProjectSetupScreen::handleCancelButtonPress()
{
	emit canceled();
}

void QtProjectSetupScreen::handleUpdateButtonPress()
{
	handleCreateButtonPress();
}

QSize QtProjectSetupScreen::sizeHint() const
{
	return QSize(600,600);
}
