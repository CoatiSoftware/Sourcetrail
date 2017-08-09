#include "qt/window/QtStartScreen.h"

#include <QCheckBox>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "utility/AppPath.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/ResourcePaths.h"
#include "utility/Version.h"

#include "License.h"
#include "PublicKey.h"
#include "qt/network/QtUpdateChecker.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

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
		MessageLoadProject(m_projectFilePath, false).dispatch();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("Missing Project File");
		msgBox.setInformativeText((
			"Couldn't find " + m_projectFilePath.str() + " in your filesystem. Delete it from this recent Proejct list?"
		).c_str());
		msgBox.addButton("Delete", QMessageBox::ButtonRole::YesRole);
		msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0) // QMessageBox::Yes
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
}


QtStartScreen::QtStartScreen(QWidget *parent)
	: QtWindow(true, parent)
{
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
			connect(button, &QtRecentProjectButton::clicked, button, &QtRecentProjectButton::handleButtonClick);
			if (button->projectExists())
			{
				button->setObjectName("recentButton");
				connect(button, &QtRecentProjectButton::clicked, this, &QtStartScreen::handleRecentButton);
			}
			else
			{
				connect(button, &QtRecentProjectButton::updateButtons, this, &QtStartScreen::updateButtons);
				button->setObjectName("recentButtonMissing");
			}
		}
		else
		{
			button->hide();
		}
		i++;
	}
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("startscreen/startscreen.css"))).c_str());
}

void QtStartScreen::setupStartScreen()
{
	License license;
	license.loadFromEncodedString(ApplicationSettings::getInstance()->getLicenseString(), AppPath::getAppPath());
	bool licenseValid = license.isValid();

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("startscreen/startscreen.css"))).c_str());
	addLogo();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(15, 170, 15, 10);
	layout->setSpacing(1);
	m_content->setLayout(layout);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 2);

		QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
		versionLabel->setObjectName("versionLabel");
		col->addWidget(versionLabel);

		QPushButton* updateButton = new QPushButton("check for new version", this);
		updateButton->setObjectName("updateButton");
		updateButton->setCursor(Qt::PointingHandCursor);
		connect(updateButton, &QPushButton::clicked,
			[]()
			{
				QtUpdateChecker::check(true);
			}
		);
		col->addWidget(updateButton);

		QCheckBox* updateCheckbox = new QCheckBox("automatic update check");
		updateCheckbox->setObjectName("updateCheckbox");
		updateCheckbox->setChecked(ApplicationSettings::getInstance()->getAutomaticUpdateCheck());
		connect(updateCheckbox, &QCheckBox::stateChanged,
			[updateCheckbox]()
			{
				ApplicationSettings::getInstance()->setAutomaticUpdateCheck(updateCheckbox->isChecked());
				ApplicationSettings::getInstance()->save();

				if (updateCheckbox->isChecked())
				{
					QtUpdateChecker::check();
				}
			}
		);
		col->addWidget(updateCheckbox);

		if (!licenseValid)
		{
			col->addSpacing(20);

			QLabel* welcomeLabel = new QLabel(
				"<b>Welcome to the trial version of Sourcetrail!</b><br />"
				"Explore our preindexed projects to experience Sourcetrail's unique user interface. "
				"More projects are available for download <a href=\"http://sourcetrail.com/downloads#extra\" style=\"color: #007AC2;\">here</a>.<br /><br />"
				"If you want to use Sourcetrail on your own source code please "
				"<a href=\"http://sourcetrail.com/buy-license\" style=\"color: #007AC2;\">purchase a license</a>, "
				"or get a temporary <a href=\"http://sourcetrail.com/test-license\" style=\"color: #007AC2;\">test license</a>.", this);
			welcomeLabel->setOpenExternalLinks(true);
			welcomeLabel->setObjectName("welcomeLabel");
			welcomeLabel->setWordWrap(true);
			welcomeLabel->setAlignment(Qt::AlignTop);

			col->addWidget(welcomeLabel, 0, Qt::AlignHCenter | Qt::AlignTop);
			col->addStrut(260);
			col->addStretch();

			QPushButton* openProjectButton = new QPushButton("Open Project", this);
			openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			openProjectButton->setObjectName("projectButton");
			openProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(openProjectButton, &QPushButton::clicked, this, &QtStartScreen::handleOpenProjectButton);
			col->addWidget(openProjectButton);

			col->addSpacing(8);

			QPushButton* unlockButton = new QPushButton("Unlock", this);
			unlockButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			unlockButton->setObjectName("projectButton");
			unlockButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(unlockButton, &QPushButton::clicked, this, &QtStartScreen::handleUnlockButton);

			QHBoxLayout* row = new QHBoxLayout();
			row->addWidget(unlockButton);
			row->addStretch();
			col->addLayout(row);

			layout->addStretch(1);
		}
		else
		{
			col->addSpacing(30);

			std::string licenseString = license.getLicenseInfo();

			QLabel* licenseHeader = new QLabel("Licensed to:");
			licenseHeader->setObjectName("licenseHeaderLabel");
			col->addWidget(licenseHeader);
			col->addSpacing(2);
			QLabel* licenseLabel = new QLabel(licenseString.c_str());
			licenseLabel->setObjectName("licenseLabel");
			col->addWidget(licenseLabel);

			col->addStretch();

			QPushButton* newProjectButton = new QPushButton("New Project", this);
			newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			newProjectButton->setObjectName("projectButton");
			newProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(newProjectButton, &QPushButton::clicked, this, &QtStartScreen::handleNewProjectButton);
			col->addWidget(newProjectButton);

			col->addSpacing(8);

			QPushButton* openProjectButton = new QPushButton("Open Project", this);
			openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			openProjectButton->setObjectName("projectButton");
			openProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			connect(openProjectButton, &QPushButton::clicked, this, &QtStartScreen::handleOpenProjectButton);
			col->addWidget(openProjectButton);

			layout->addStretch(2);
		}
	}

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 1);

		QLabel* recentProjectsLabel = new QLabel("Recent Projects: ", this);
		if (!licenseValid)
		{
			recentProjectsLabel->setText("Projects:");
		}
		recentProjectsLabel->setObjectName("recentLabel");
		col->addWidget(recentProjectsLabel);

		col->addSpacing(20);

		m_cppIcon = new QIcon((ResourcePaths::getGuiPath().str() + "icon/cpp_icon.png").c_str());
		m_cIcon = new QIcon((ResourcePaths::getGuiPath().str() + "icon/c_icon.png").c_str());
		m_javaIcon = new QIcon((ResourcePaths::getGuiPath().str() + "icon/java_icon.png").c_str());
		m_projectIcon = new QIcon((ResourcePaths::getGuiPath().str() + "icon/empty_icon.png").c_str());
		for (int i = 0
			; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount()
			; i++)
		{
			QtRecentProjectButton* button = new QtRecentProjectButton(this);
			button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			button->setIcon(*m_projectIcon);
			button->setIconSize(QSize(30, 30));
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
		parentWidget()->width() / 2 - size.width() / 2,
		parentWidget()->height() / 2 - size.height() / 2
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
