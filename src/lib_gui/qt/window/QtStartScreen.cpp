#include "qt/window/QtStartScreen.h"

#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/logging/logging.h"
#include "isTrial.h"

QtRecentProjectButton::QtRecentProjectButton(QWidget* parent)
	: QPushButton(parent)
{
}

bool QtRecentProjectButton::projectExists() const
{
	return m_projectExists;
}

void QtRecentProjectButton::setProjectPath(const FilePath& projectFilePath)
{
	m_projectFilePath = projectFilePath;
	m_projectExists = projectFilePath.exists();
	this->setText(m_projectFilePath.withoutExtension().fileName().c_str());
	if (m_projectExists)
	{
		this->setToolTip(m_projectFilePath.str().c_str());
	}
	else
	{
		std::string missingFileText = "Couldn't find " + m_projectFilePath.str() + " in your filesystem";
		this->setToolTip(missingFileText.c_str());
	}
}

void QtRecentProjectButton::handleButtonClick()
{
	if (m_projectExists)
	{
		MessageDispatchWhenLicenseValid(
			std::make_shared<MessageLoadProject>(m_projectFilePath.str(), false)
		).dispatch();
	}
	else
	{
		std::string text = "Couldn't find " + m_projectFilePath.str()
			+ " in your filesystem. Delete it from this recent Proejct list?";
		int ret = QMessageBox::question(this, "Missing Project File", text.c_str(), QMessageBox::Yes | QMessageBox::No);

		if (ret == QMessageBox::Yes)
		{
			std::vector<FilePath> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();
			const int maxRecentProjectsCount = ApplicationSettings::getInstance()->getMaxRecentProjectsCount();
			for (int i = 0; i < maxRecentProjectsCount; i++)
			{
				if (recentProjects[i].str() == m_projectFilePath.str())
				{
					recentProjects.erase(recentProjects.begin()+i);
					ApplicationSettings::getInstance()->setRecentProjects(recentProjects);
					ApplicationSettings::getInstance()->save();
					break;
				}
			}
			emit updateButtons();
		}
	}
};


QtStartScreen::QtStartScreen(QWidget *parent)
	: QtWindow(parent)
{
	this->raise();
}

QSize QtStartScreen::sizeHint() const
{
	return QSize(570, 600);
}

void QtStartScreen::updateButtons()
{
	std::vector<FilePath> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();
	size_t i = 0;
	for (QtRecentProjectButton* button : m_recentProjectsButtons)
	{
		button->disconnect();
		if (i < recentProjects.size())
		{
			button->setProjectPath(recentProjects[i]);
			button->setFixedWidth(button->fontMetrics().width(button->text()) + 45);
			connect(button, SIGNAL(clicked()), button, SLOT(handleButtonClick()));
			if (button->projectExists())
			{
				button->setObjectName("recentButton");
				connect(button, SIGNAL(clicked()), this, SLOT(handleRecentButton()));
			}
			else
			{
				connect(button, SIGNAL(updateButtons()), this, SLOT(updateButtons()));
				button->setObjectName("recentButtonMissing");
			}
		}
		else
		{
			button->hide();
		}
		i++;
	}
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "startscreen/startscreen.css").c_str());
}

void QtStartScreen::setupStartScreen()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "startscreen/startscreen.css").c_str());
	addLogo();

	if (!isTrial())
	{
		m_newProjectButton = new QPushButton("New Project", this);
		m_newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		m_newProjectButton->move(30, 505);
		m_newProjectButton->setObjectName("projectButton");
		m_newProjectButton->show();
		connect(m_newProjectButton, SIGNAL(clicked()), this, SLOT(handleNewProjectButton()));

		m_openProjectButton = new QPushButton("Open Project", this);
		m_openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		m_openProjectButton->move(30, 540);
		m_openProjectButton->setObjectName("projectButton");
		m_openProjectButton->show();
		connect(m_openProjectButton, SIGNAL(clicked()), this, SLOT(handleOpenProjectButton()));
	}

	QLabel* recentProjectsLabel = new QLabel("Recent Projects: ", this);
	recentProjectsLabel->setGeometry(300, 234, 300, 50);
	recentProjectsLabel->setObjectName("recentLabel");
	recentProjectsLabel->show();

	int position = 290;
	QIcon cpp_icon((ResourcePaths::getGuiPath() + "icon/project_256_256.png").c_str());
	for (int i = 0
		; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount()
		; i++)
	{
		QtRecentProjectButton* button = new QtRecentProjectButton(this);
		button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		button->setIcon(cpp_icon);
		button->setIconSize(QSize(25, 25));
		button->setObjectName("recentButtonMissing");
		button->setGeometry(292, position, button->fontMetrics().width(button->text()) + 45, 40);
		button->minimumSizeHint(); // force font loading
		button->show();
		position += 40;
		m_recentProjectsButtons.push_back(button);
	}
	updateButtons();
}

void QtStartScreen::handleNewProjectButton()
{
	emit openNewProjectDialog();
}

void QtStartScreen::handleOpenProjectButton()
{
	emit openOpenProjectDialog();
}

void QtStartScreen::handleRecentButton()
{
	emit finished();
}
