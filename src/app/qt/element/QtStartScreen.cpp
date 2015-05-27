#include <QtWidgets/qlabel.h>
#include "QtStartScreen.h"

#include <QPainter>
#include <QKeyEvent>

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageLoadProject.h"

QtRecentProjectButton::QtRecentProjectButton(const QString &text, QWidget *parent)
	: QPushButton(text, parent)
{
	m_projectFile = text.toStdString();
	this->setText(FileSystem::fileName(FileSystem::filePathWithoutExtension(text.toStdString())).c_str());
}

void QtRecentProjectButton::handleButtonClick()
{
	MessageLoadProject(m_projectFile).dispatch();
	parentWidget()->hide();
};

QtStartScreen::QtStartScreen(QWidget *parent)
	: QWidget(parent, Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	setGeometry(parent->pos().x()+parent->width()/2-350,parent->pos().y()+parent->height()/2-250,600,500);

	QPalette palette;
	QLinearGradient linearGradient(QPointF(0, 0), QPointF(0, 500));
	linearGradient.setColorAt(0, QColor(46,61,135));
	linearGradient.setColorAt(1, QColor(0,120,195));
	palette.setBrush(QPalette::Window,*(new QBrush(linearGradient)));
	this->setPalette(palette);

	//QPixmap coati_logo("data/gui/logo_coati.png");
	QPixmap coati_logo("data/gui/startscreen/logo_white.png");
	QPixmap cpp_icon("data/gui/startscreen/cpluspluss_icon.png");

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coati_logo);
	coatiLogoLabel->setGeometry(30,30,100,138);

	std::string buttonStyle =
			"QPushButton{font-family: "
				"Source Code Pro; "
				"font-size: 17pt; "
				"border-radius: 15px; "
				"background: white} "
			"QPushButton:hover {background:lightgrey;}";

	m_newProjectButton = new QPushButton("New Project", this);
	m_newProjectButton->setGeometry(20,350, 200, 50);
	m_newProjectButton->setStyleSheet(buttonStyle.c_str());
	connect(m_newProjectButton, SIGNAL(clicked()), this, SLOT(handleNewProjectButton()));
	m_openProjectButton = new QPushButton("Open Project", this);
	m_openProjectButton->setGeometry(20,420, 200, 50);
	m_openProjectButton->setStyleSheet(buttonStyle.c_str());
	connect(m_openProjectButton, SIGNAL(clicked()), this, SLOT(handleOpenProjectButton()));

	connect(this, SIGNAL(openOpenProjectDialog()), parent, SLOT(openProject()));
	connect(this, SIGNAL(openNewProjectDialog()), parent, SLOT(newProject()));

	QLabel* recentProjectsLabel = new QLabel("RECENT PROJECTS: ", this);
	QFont font("Helvetica", 20);
	recentProjectsLabel->setFont(font);
	recentProjectsLabel->setStyleSheet("QLabel{font-family: Source Code Pro; font-size: 20pt; color: white;}");
	recentProjectsLabel->setGeometry(250, 100, 300, 50);

	std::vector<std::string> extensions;
	extensions.push_back(".xml");
	std::vector<std::string> recentProjects = FileSystem::getFileNamesFromDirectory("data/projects", extensions);

	int position = 150;
	for(std::string project : recentProjects)
	{
		QtRecentProjectButton* button = new QtRecentProjectButton(project.c_str(), this);
		button->setGeometry(250, position, 400, 40);
		button->setIcon(cpp_icon);
		button->setIconSize(QSize(40,40));
		button->setStyleSheet(
				"QPushButton{"
						"font-family: Source Code Pro; "
						"font-size: 12pt; "
						"Text-align:left; "
						"color: white; "
						"background:none; "
						"border:none;} "
				"QPushButton:hover{color:lightgrey;}");
		connect(button, SIGNAL(clicked()), button, SLOT(handleButtonClick()));
		position +=50;
	}

	this->raise();

}

QSize QtStartScreen::sizeHint() const
{
	return QSize(500,500);
}

void QtStartScreen::paintEvent(QPaintEvent *event)
{
	QPainter* painter = new QPainter(this);
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
