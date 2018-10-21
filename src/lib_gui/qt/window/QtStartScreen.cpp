#include "QtStartScreen.h"

#include <QCheckBox>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "ApplicationSettings.h"
#include "AppPath.h"
#include "License.h"
#include "MessageLoadProject.h"
#include "ProjectSettings.h"
#include "PublicKey.h"
#include "QtUpdateCheckerWidget.h"
#include "QtNewsWidget.h"
#include "ResourcePaths.h"
#include "utilityQt.h"
#include "Version.h"

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
	this->setText(QString::fromStdWString(m_projectFilePath.withoutExtension().fileName()));
	if (m_projectExists)
	{
		this->setToolTip(QString::fromStdWString(m_projectFilePath.wstr()));
	}
	else
	{
		const std::wstring missingFileText = L"Couldn't find " + m_projectFilePath.wstr() + L" in your filesystem";
		this->setToolTip(QString::fromStdWString(missingFileText));
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
		msgBox.setInformativeText(QString::fromStdWString(
			L"<p>Couldn't find \"" + m_projectFilePath.wstr() + L"\" on your filesystem.</p><p>Do you want to remove it from recent project list?</p>"
		));
		msgBox.addButton("Remove", QMessageBox::ButtonRole::YesRole);
		msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0) // QMessageBox::Yes
		{
			std::vector<FilePath> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();
			const int maxRecentProjectsCount = ApplicationSettings::getInstance()->getMaxRecentProjectsCount();
			for (int i = 0; i < maxRecentProjectsCount; i++)
			{
				if (recentProjects[i].wstr() == m_projectFilePath.wstr())
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
	return QSize(570, 630);
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
	layout->setContentsMargins(15, 170, 15, 0);
	layout->setSpacing(1);
	m_content->setLayout(layout);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 3);

		QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
		versionLabel->setObjectName("boldLabel");
		col->addWidget(versionLabel);

		QtUpdateCheckerWidget* checker = new QtUpdateCheckerWidget(this);
		col->addWidget(checker);

		col->addSpacing(15);

		if (license.isValid())
		{
			std::string licenseString = license.getLicenseInfo();

			QLabel* licenseHeader = new QLabel("Licensed to:");
			licenseHeader->setObjectName("boldLabel");
			col->addWidget(licenseHeader);
			col->addSpacing(2);
			QLabel* licenseLabel = new QLabel(licenseString.c_str());
			licenseLabel->setObjectName("textLabel");
			col->addWidget(licenseLabel);
		}
		else
		{
			QLabel* licenseHeader = new QLabel("Not licensed for<br />commercial use.");
			licenseHeader->setObjectName("boldLabel");
			col->addWidget(licenseHeader);

			QPushButton* upgradeButton = new QPushButton("upgrade");
			upgradeButton->setObjectName("upgradeButton");
			upgradeButton->setCursor(Qt::PointingHandCursor);
			col->addWidget(upgradeButton);
			connect(upgradeButton, &QPushButton::clicked, [this](){ emit openEnterLicenseDialog(); });
		}

		col->addSpacing(15);

		{
			QLabel* newsHeader = new QLabel("News:");
			newsHeader->setObjectName("boldLabel");
			col->addWidget(newsHeader);

			QtNewsWidget* newsWidget = new QtNewsWidget(this);
			col->addWidget(newsWidget);

			connect(checker, &QtUpdateCheckerWidget::updateReceived, newsWidget, &QtNewsWidget::updateNews);
		}

		col->addSpacing(15);
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
	}

	layout->addSpacing(50);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 1);

		QLabel* recentProjectsLabel = new QLabel("Recent Projects: ", this);
		recentProjectsLabel->setObjectName("titleLabel");
		col->addWidget(recentProjectsLabel);

		col->addSpacing(20);

		for (int i = 0; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount(); i++)
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
