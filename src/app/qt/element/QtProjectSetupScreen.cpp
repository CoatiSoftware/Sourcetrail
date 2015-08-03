#include "qt/element/QtProjectSetupScreen.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>


#include "settings/ProjectSettings.h"
#include "utility/logging/logging.h"
#include "qt/utility/QtDeviceScaledPixmap.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "qt/utility/utilityQt.h"

QtTextLine::QtTextLine(QWidget *parent)
	:QWidget(parent)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	setLayout(layout);

	m_data = new QLineEdit(this);
	m_button = new QPushButton("...");
	m_button->setMaximumWidth(40);
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
	QString file = QFileDialog::getExistingDirectory(this, tr("Select File"), "");
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
	setStyleSheet(utility::getStyleSheet("data/gui/settingwindows/projectsetup.css").c_str());
	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(20,30,20,20);
	QLabel* windowTitleLabel = new QLabel("NEW PROJECT");
	windowLayout->addWidget(windowTitleLabel);
	windowLayout->addSpacing(30);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setObjectName("coatiBackground");
	QWidget* form = new QWidget();
	form->setObjectName("form");
	scrollArea->setWidget(form);
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setWidgetResizable(true);
	QFormLayout *layout = new QFormLayout();
	layout->setContentsMargins(20,30,20,20);
	
	//Background Image
	QPalette p = palette();
	QPixmap pixmap1("data/gui/startscreen/logo.png");
	QRect rect = scrollArea->rect();
	QSize size(rect.width()-90,rect.height());
	QPixmap pixmap(pixmap1.scaled(size));
	p.setBrush(QPalette::Background,  pixmap);
	scrollArea->setPalette(p);

	QLabel* nameLabel = new QLabel("Name");
	m_projectName = new QLineEdit();
	layout->addRow(nameLabel, m_projectName);

	QLabel* locationLabel = new QLabel("Location");
	m_projectFileLocation = new QtTextLine(this);
	layout->addRow(locationLabel, m_projectFileLocation);

	/*
	QLabel* languageLabel = new QLabel("Language");
	QComboBox* language = new QComboBox();
	language->insertItem(0, "C++");
	layout->addRow(languageLabel, language);
	 */

	QLabel* sourcePathsLabel = new QLabel("Sourcepaths");
	m_sourcePaths = new QtDirectoryListBox(this);
	layout->addRow(sourcePathsLabel, m_sourcePaths);

	QLabel* includePathsLabel = new QLabel("IncludePaths");
	m_includePaths = new QtDirectoryListBox(this);
	layout->addRow(includePathsLabel, m_includePaths);

	if(QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		QLabel* frameworkPathsLabel = new QLabel("FrameworkPaths");
		m_frameworkPaths = new QtDirectoryListBox(this);
		layout->addRow(frameworkPathsLabel, m_frameworkPaths);
	}
	form->setLayout(layout);

	QPushButton* createButton = new QPushButton("Create");
	createButton->setObjectName("windowButton");
	QPushButton* cancelButton = new QPushButton("Cancel");
	cancelButton->setObjectName("windowButton");

	connect(createButton, SIGNAL(clicked()), this, SLOT(handleCreateButtonPress()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(handleCancelButtonPress()));

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addSpacing(100);
	buttons->addWidget(createButton);
	buttons->addWidget(cancelButton);
	buttons->addSpacing(30);

	windowLayout->addWidget(scrollArea);
	windowLayout->addSpacing(20);
	windowLayout->addLayout(buttons);

	m_window->setLayout(windowLayout);
	resize(QSize(600,600));
}

void QtProjectSetupScreen::handleCancelButtonPress()
{
	hide();
}

void QtProjectSetupScreen::handleCreateButtonPress()
{
	bool error = false;
	if(m_projectName->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please Enter a Project Name");
		msgBox.exec();
		error = true;
	}
	if(m_projectFileLocation->getText().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Location for the ProjectFiles is empty");
		msgBox.exec();
		error = true;
	}

	if(!error)
	{	
		ProjectSettings::getInstance()->setSourcePaths(m_sourcePaths->getList());
		ProjectSettings::getInstance()->setHeaderSearchPaths(m_includePaths->getList());
		if(m_frameworkPaths)
		{
			ProjectSettings::getInstance()->setFrameworkSearchPaths(m_frameworkPaths->getList());
		}
		std::string projectFile =
				m_projectFileLocation->getText().toStdString() + "/" + m_projectName->text().toStdString() + ".xml";
		ProjectSettings::getInstance()->save(projectFile);
		MessageLoadProject(projectFile).dispatch();
	}
}

QSize QtProjectSetupScreen::sizeHint() const
{
	return QSize(600,600);
}
