#include "qt/window/QtStartScreen.h"

#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/logging/logging.h"
#include "utility/Version.h"

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
		MessageLoadProject(m_projectFilePath.str(), false).dispatch();
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
			LanguageType lang = ProjectSettings::getLanguageOfProject(recentProjects[i]);
			switch (lang)
			{
				case LanguageType::LANGUAGE_C:
					button->setIcon(*m_cIcon);
					break;
				case LANGUAGE_CPP:
					button->setIcon(*m_cppIcon);
					break;
				case LANGUAGE_JAVA:
					button->setIcon(*m_javaIcon);
					break;
				default:
					button->setIcon(*m_projectIcon);
					break;
			}
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

void QtStartScreen::setupStartScreen(bool unlocked)
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "startscreen/startscreen.css").c_str());
	addLogo();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(15, 170, 15, 10);
	layout->setSpacing(1);
	m_content->setLayout(layout);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 2);

		if (!unlocked)
		{
			col->addSpacing(50);

			QLabel* welcomeLabel = new QLabel(
				"Welcome to the <b>Coati trial</b>!<br /><br />"
				"Explore our preindexed projects to experience Coati's unique user interface.<br /><br />"
				"If you want to use Coati on your own source code please "
				"<a href=\"http://coati.io/buy-license\" style=\"color: #007AC2;\">purchase a license</a>, "
				"or ask us for a temporary test license by writing to <b>support@coati.io</b>.", this);
			welcomeLabel->setOpenExternalLinks(true);
			welcomeLabel->setObjectName("welcomeLabel");
			welcomeLabel->setWordWrap(true);
			welcomeLabel->setAlignment(Qt::AlignTop);

			col->addWidget(welcomeLabel, 0, Qt::AlignHCenter | Qt::AlignTop);
			col->addStretch();

			QPushButton* unlockButton = new QPushButton("Unlock", this);
			unlockButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			unlockButton->setObjectName("projectButton");
			unlockButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(unlockButton, SIGNAL(clicked()), this, SLOT(handleUnlockButton()));
			col->addWidget(unlockButton);

			QHBoxLayout* row = new QHBoxLayout();
			row->addWidget(unlockButton);
			row->addStretch();
			col->addLayout(row);

			layout->addStretch(1);
		}
		else
		{
			QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
			versionLabel->setObjectName("versionLabel");
			col->addWidget(versionLabel);

			QLabel* updateLabel = new QLabel(
				"<a href=\"https://coati.io/downloads\" style=\"color: #007AC2;\">check for new version</a>", this);
			updateLabel->setOpenExternalLinks(true);
			updateLabel->setObjectName("updateLabel");
			col->addWidget(updateLabel);

			col->addStretch();

			QPushButton* newProjectButton = new QPushButton("New Project", this);
			newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			newProjectButton->setObjectName("projectButton");
			newProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(newProjectButton, SIGNAL(clicked()), this, SLOT(handleNewProjectButton()));
			col->addWidget(newProjectButton);

			col->addSpacing(8);

			QPushButton* openProjectButton = new QPushButton("Open Project", this);
			openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			openProjectButton->setObjectName("projectButton");
			openProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(openProjectButton, SIGNAL(clicked()), this, SLOT(handleOpenProjectButton()));
			col->addWidget(openProjectButton);

			layout->addStretch(2);
		}
	}

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 1);

		QLabel* recentProjectsLabel = new QLabel("Recent Projects: ", this);
		if (!unlocked)
		{
			recentProjectsLabel->setText("Projects:");
		}
		recentProjectsLabel->setObjectName("recentLabel");
		col->addWidget(recentProjectsLabel);

		col->addSpacing(20);

		m_cppIcon = new QIcon((ResourcePaths::getGuiPath() + "icon/cpp_icon.png").c_str());
		m_cIcon = new QIcon((ResourcePaths::getGuiPath() + "icon/c_icon.png").c_str());
		m_javaIcon = new QIcon((ResourcePaths::getGuiPath() + "icon/java_icon.png").c_str());
		m_projectIcon = new QIcon((ResourcePaths::getGuiPath() + "icon/project_256_256.png").c_str());
		for (int i = 0
			; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount()
			; i++)
		{
			QtRecentProjectButton* button = new QtRecentProjectButton(this);
			button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			button->setIcon(*m_projectIcon);
			button->setIconSize(QSize(25, 25));
			button->setMinimumSize(button->fontMetrics().width(button->text()) + 45, 40);
			button->setObjectName("recentButtonMissing");
			button->minimumSizeHint(); // force font loading
			m_recentProjectsButtons.push_back(button);
			col->addWidget(button);
		}

		col->addStretch();
	}

	layout->addStretch(1);

	updateButtons();

	QSize size = sizeHint();
	move(
		parentWidget()->pos().x() + parentWidget()->width() / 2 - size.width() / 2,
		parentWidget()->pos().y() + parentWidget()->height() / 2 - size.height() / 2
	);
}

void QtStartScreen::handleNewProjectButton()
{
	emit openNewProjectDialog();
}

void QtStartScreen::handleOpenProjectButton()
{
	emit openOpenProjectDialog();
}

void QtStartScreen::handleUnlockButton()
{
	emit openEnterLicenseDialog();
}

void QtStartScreen::handleRecentButton()
{
	emit finished();
}
