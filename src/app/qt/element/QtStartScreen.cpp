#include "qt/element/QtStartScreen.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageLoadProject.h"

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"

QtRecentProjectButton::QtRecentProjectButton(const QString &text, QWidget *parent)
	: QPushButton(text, parent)
{
	m_projectFile = text.toStdString();
	this->setText(FileSystem::fileName(text.toStdString()).c_str());
	this->setToolTip(m_projectFile.c_str());
}

void QtRecentProjectButton::handleButtonClick()
{
	MessageLoadProject(m_projectFile).dispatch();
	parentWidget()->hide();
};

QtStartScreen::QtStartScreen(QWidget *parent)
	: QtSettingsWindow(parent, 50)
{
	this->raise();
}

void QtStartScreen::setup()
{
	setStyleSheet(utility::getStyleSheet("data/gui/startscreen/startscreen.css").c_str());
	std::vector<std::string> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();

	QtDeviceScaledPixmap coati_logo("data/gui/startscreen/logo_schriftzug.png");
	coati_logo.scaleToWidth(250);
	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coati_logo.pixmap());
	coatiLogoLabel->resize(coati_logo.width(), coati_logo.height());
	coatiLogoLabel->move(30,10);

	m_newProjectButton = new QPushButton("New Project", this);
	m_newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_newProjectButton->setGeometry(30, 375, 140, 30);
	m_newProjectButton->setObjectName("projectButton");
	connect(m_newProjectButton, SIGNAL(clicked()), this, SLOT(handleNewProjectButton()));

	m_openProjectButton = new QPushButton("Open Project", this);
	m_openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_openProjectButton->setGeometry(30, 420, 140, 30);
	m_openProjectButton->setObjectName("projectButton");
	connect(m_openProjectButton, SIGNAL(clicked()), this, SLOT(handleOpenProjectButton()));

	connect(this, SIGNAL(openOpenProjectDialog()), parent(), SLOT(openProject()));
	connect(this, SIGNAL(openNewProjectDialog()), parent(), SLOT(newProject()));

	QLabel* recentProjectsLabel = new QLabel("RECENT PROJECTS: ", this);
	recentProjectsLabel->setGeometry(300, 129, 300, 50);
	recentProjectsLabel->setObjectName("recentLabel");

	int position = 200;
	QIcon cpp_icon("data/gui/startscreen/Icon_CPP.png");
	for (std::string project : recentProjects)
	{
		if(FileSystem::exists(project.c_str()))
		{
			QtRecentProjectButton* button = new QtRecentProjectButton(project.c_str(), this);
			button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			button->setIcon(cpp_icon);
			button->setIconSize(QSize(25, 25));
			button->setObjectName("recentButton");
			button->minimumSizeHint(); // force font loading
			button->setGeometry(292, position, button->fontMetrics().width(button->text()) + 45, 40);
			connect(button, SIGNAL(clicked()), button, SLOT(handleButtonClick()));
			position += 40;
		}
	}

	resize(600,500);
}

QSize QtStartScreen::sizeHint() const
{
	return QSize(500, 500);
}

void QtStartScreen::handleNewProjectButton()
{
	this->hide();
	emit openNewProjectDialog();
}

void QtStartScreen::handleOpenProjectButton()
{
	this->hide();
	emit openOpenProjectDialog();
}
