#include "QtStartScreen.h"

#include <QLabel>

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageLoadProject.h"

#include "qt/utility/QtDeviceScaledPixmap.h"

QtRecentProjectButton::QtRecentProjectButton(const QString &text, QWidget *parent)
	: QPushButton(text, parent)
{
	m_projectFile = text.toStdString();
	this->setText(FileSystem::fileName(text.toStdString()).c_str());
}

void QtRecentProjectButton::handleButtonClick()
{
	MessageLoadProject(m_projectFile).dispatch();
	parentWidget()->hide();
};

QtStartScreen::QtStartScreen(QWidget *parent)
	: QWidget(parent, Qt::Dialog | Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	setGeometry(parent->pos().x()+parent->width()/2-250,parent->pos().y()+parent->height()/2-250,500,500);

	QPalette palette;
	QLinearGradient linearGradient(QPointF(0, 275), QPointF(0, 500));
	linearGradient.setColorAt(0, QColor(46, 61, 135));
	linearGradient.setColorAt(1, QColor(0, 120, 195));
	palette.setBrush(QPalette::Window, *(new QBrush(linearGradient)));
	this->setPalette(palette);

	QtDeviceScaledPixmap coati_logo("data/gui/startscreen/logo_white.png");
	coati_logo.scaleToWidth(100);

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coati_logo.pixmap());
	coatiLogoLabel->setGeometry(30, 30, coati_logo.width(), coati_logo.height());

	std::string buttonStyle =
		"QPushButton {"
			"font-size: 17pt; "
			"border-radius: 10px; "
			"background: white; "
		"} "
		"QPushButton:hover {"
			"background: rgba(255, 255, 255, 0.8); "
		"}";

	m_newProjectButton = new QPushButton("New Project", this);
	m_newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_newProjectButton->setGeometry(30, 375, 140, 30);
	m_newProjectButton->setStyleSheet(buttonStyle.c_str());
	connect(m_newProjectButton, SIGNAL(clicked()), this, SLOT(handleNewProjectButton()));

	m_openProjectButton = new QPushButton("Open Project", this);
	m_openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_openProjectButton->setGeometry(30, 420, 140, 30);
	m_openProjectButton->setStyleSheet(buttonStyle.c_str());
	connect(m_openProjectButton, SIGNAL(clicked()), this, SLOT(handleOpenProjectButton()));

	connect(this, SIGNAL(openOpenProjectDialog()), parent, SLOT(openProject()));
	connect(this, SIGNAL(openNewProjectDialog()), parent, SLOT(newProject()));

	QLabel* recentProjectsLabel = new QLabel("RECENT PROJECTS: ", this);
	recentProjectsLabel->setGeometry(300, 129, 300, 50);
	recentProjectsLabel->setStyleSheet(
		"QLabel {"
			"font-size: 16pt; "
			"color: white; "
		"}"
	);

	std::vector<std::string> extensions;
	extensions.push_back(".xml");
	std::vector<std::string> recentProjects = FileSystem::getFileNamesFromDirectory("data/projects", extensions);

	int position = 200;
	QIcon cpp_icon("data/gui/startscreen/cpluspluss_icon.png");
	for (std::string project : recentProjects)
	{
		QtRecentProjectButton* button = new QtRecentProjectButton(project.c_str(), this);
		button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		button->setIcon(cpp_icon);
		button->setIconSize(QSize(25, 25));
		button->setStyleSheet(
			"QPushButton{"
					"font-size: 12pt; "
					// "text-align: left; " // when using this the QPixmap is drawn without regard of the screens DPI
					"color: white; "
					"background: none; "
					"border: none; "
			"} "
			"QPushButton:hover{"
				"background: rgba(255, 255, 255, 0.15); "
				"border-radius: 10px; "
			"}"
		);
		button->minimumSizeHint(); // force font loading
		button->setGeometry(292, position, button->fontMetrics().width(button->text()) + 45, 40);
		connect(button, SIGNAL(clicked()), button, SLOT(handleButtonClick()));
		position += 40;
	}

	this->raise();
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
