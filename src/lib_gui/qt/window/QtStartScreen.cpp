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
#include "qt/element/QtUpdateCheckerWidget.h"
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
	this->setText(QString::fromStdWString(m_projectFilePath.withoutExtension().wFileName()));
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
		MessageLoadProject(m_projectFilePath).dispatch();
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
	, m_cppIcon(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/cpp_icon.png").wstr()))
	, m_cIcon(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/c_icon.png").wstr()))
	, m_javaIcon(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/java_icon.png").wstr()))
	, m_projectIcon(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/empty_icon.png").wstr()))
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
					button->setIcon(m_cIcon);
					break;
				case LANGUAGE_CPP:
					button->setIcon(m_cppIcon);
					break;
				case LANGUAGE_JAVA:
					button->setIcon(m_javaIcon);
					break;
				default:
					button->setIcon(m_projectIcon);
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
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"startscreen/startscreen.css")).c_str());
}

void QtStartScreen::setupStartScreen()
{
	License license;
	license.loadFromEncodedString(ApplicationSettings::getInstance()->getLicenseString(), AppPath::getAppPath().str());

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"startscreen/startscreen.css")).c_str());
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

		QtUpdateCheckerWidget* checker = new QtUpdateCheckerWidget(this);
		col->addWidget(checker);

		col->addSpacing(30);

		if (license.isValid())
		{
			std::string licenseString = license.getLicenseInfo();

			QLabel* licenseHeader = new QLabel("Licensed to:");
			licenseHeader->setObjectName("licenseHeaderLabel");
			col->addWidget(licenseHeader);
			col->addSpacing(2);
			QLabel* licenseLabel = new QLabel(licenseString.c_str());
			licenseLabel->setObjectName("licenseLabel");
			col->addWidget(licenseLabel);
		}
		else
		{
			QLabel* licenseHeader = new QLabel("Not licensed for<br />commercial use.");
			licenseHeader->setObjectName("licenseHeaderLabel");
			col->addWidget(licenseHeader);

			QPushButton* upgradeButton = new QPushButton("upgrade");
			upgradeButton->setObjectName("upgradeButton");
			upgradeButton->setCursor(Qt::PointingHandCursor);
			col->addWidget(upgradeButton);
			connect(upgradeButton, &QPushButton::clicked, [this](){ emit openEnterLicenseDialog(); });
		}

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

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 1);

		QLabel* recentProjectsLabel = new QLabel("Recent Projects: ", this);
		recentProjectsLabel->setObjectName("recentLabel");
		col->addWidget(recentProjectsLabel);

		col->addSpacing(20);

		for (int i = 0
			; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount()
			; i++)
		{
			QtRecentProjectButton* button = new QtRecentProjectButton(this);
			button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
			button->setIcon(m_projectIcon);
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
