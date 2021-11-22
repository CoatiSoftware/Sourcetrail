#include "QtStartScreen.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <QVBoxLayout>

#include "ApplicationSettings.h"
#include "MessageLoadProject.h"
#include "ProjectSettings.h"
#include "ResourcePaths.h"
#include "Version.h"
#include "utilityQt.h"

QtRecentProjectButton::QtRecentProjectButton(QWidget* parent): QPushButton(parent) {}

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
		const std::wstring missingFileText = L"Couldn't find " + m_projectFilePath.wstr() +
			L" in your filesystem";
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
		msgBox.setText(QStringLiteral("Missing Project File"));
		msgBox.setInformativeText(QString::fromStdWString(
			L"<p>Couldn't find \"" + m_projectFilePath.wstr() +
			L"\" on your filesystem.</p><p>Do you want to remove it from recent project "
			L"list?</p>"));
		msgBox.addButton(QStringLiteral("Remove"), QMessageBox::ButtonRole::YesRole);
		msgBox.addButton(QStringLiteral("Keep"), QMessageBox::ButtonRole::NoRole);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0)	 // QMessageBox::Yes
		{
			std::vector<FilePath> recentProjects =
				ApplicationSettings::getInstance()->getRecentProjects();
			for (size_t i = 0; i < recentProjects.size(); ++i)
			{
				if (recentProjects[i].wstr() == m_projectFilePath.wstr())
				{
					recentProjects.erase(recentProjects.begin() + i);
					ApplicationSettings::getInstance()->setRecentProjects(recentProjects);
					ApplicationSettings::getInstance()->save();
					break;
				}
			}
			emit updateButtons();
		}
	}
}

QtStartScreen::QtStartScreen(QWidget* parent)
	: QtWindow(true, parent)
	, m_cppIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"icon/cpp_icon.png").wstr()))
	, m_cIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"icon/c_icon.png").wstr()))
	, m_pythonIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"icon/python_icon.png").wstr()))
	, m_javaIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"icon/java_icon.png").wstr()))
	, m_projectIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"icon/empty_icon.png").wstr()))
	, m_githubIcon(QString::fromStdWString(
		  ResourcePaths::getGuiDirectoryPath().concatenate(L"startscreen/github_icon.png").wstr()))
{
}

QSize QtStartScreen::sizeHint() const
{
	return QSize(600, 650);
}

void QtStartScreen::updateButtons()
{
	std::vector<FilePath> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();
	size_t i = 0;
	for (QtRecentProjectButton* button: m_recentProjectsButtons)
	{
		button->disconnect();
		if (i < recentProjects.size())
		{
			button->setProjectPath(recentProjects[i]);
			LanguageType lang = ProjectSettings::getLanguageOfProject(recentProjects[i]);
			switch (lang)
			{
#if BUILD_CXX_LANGUAGE_PACKAGE
			case LanguageType::LANGUAGE_C:
				button->setIcon(m_cIcon);
				break;
			case LANGUAGE_CPP:
				button->setIcon(m_cppIcon);
				break;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
			case LANGUAGE_JAVA:
				button->setIcon(m_javaIcon);
				break;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
			case LANGUAGE_PYTHON:
				button->setIcon(m_pythonIcon);
				break;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
			case LANGUAGE_CUSTOM:
			default:
				button->setIcon(m_projectIcon);
				break;
			}
			button->setFixedWidth(button->fontMetrics().boundingRect(button->text()).width() + 45);
			connect(
				button,
				&QtRecentProjectButton::clicked,
				button,
				&QtRecentProjectButton::handleButtonClick);
			if (button->projectExists())
			{
				button->setObjectName(QStringLiteral("recentButton"));
				connect(
					button, &QtRecentProjectButton::clicked, this, &QtStartScreen::handleRecentButton);
			}
			else
			{
				connect(
					button, &QtRecentProjectButton::updateButtons, this, &QtStartScreen::updateButtons);
				button->setObjectName(QStringLiteral("recentButtonMissing"));
			}
		}
		else
		{
			button->hide();
		}
		i++;
	}
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiDirectoryPath().concatenate(
											 L"startscreen/startscreen.css"))
					  .c_str());
}

void QtStartScreen::setupStartScreen()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiDirectoryPath().concatenate(
											 L"startscreen/startscreen.css"))
					  .c_str());
	addLogo();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(15, 170, 15, 0);
	layout->setSpacing(1);
	m_content->setLayout(layout);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 3);

		QLabel* versionLabel = new QLabel(
			("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
		versionLabel->setObjectName(QStringLiteral("boldLabel"));
		col->addWidget(versionLabel);

		col->addSpacing(20);

		QPushButton* githubButton = new QPushButton(QStringLiteral("View on GitHub"), this);
		githubButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		githubButton->setObjectName(QStringLiteral("infoButton"));
		githubButton->setIcon(m_githubIcon);
		githubButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(
			githubButton,
			&QPushButton::clicked,
			[]()
			{
				QDesktopServices::openUrl(QUrl(
					QStringLiteral("https://github.com/CoatiSoftware/Sourcetrail"),
					QUrl::TolerantMode));
			});
		col->addWidget(githubButton);

		col->addSpacing(35);
		col->addStretch();

		QPushButton* newProjectButton = new QPushButton(QStringLiteral("New Project"), this);
		newProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
		newProjectButton->setObjectName(QStringLiteral("projectButton"));
		newProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(
			newProjectButton, &QPushButton::clicked, this, &QtStartScreen::handleNewProjectButton);
		col->addWidget(newProjectButton);

		col->addSpacing(8);

		QPushButton* openProjectButton = new QPushButton(QStringLiteral("Open Project"), this);
		openProjectButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);	 // fixes layouting on Mac
		openProjectButton->setObjectName(QStringLiteral("projectButton"));
		openProjectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(
			openProjectButton, &QPushButton::clicked, this, &QtStartScreen::handleOpenProjectButton);
		col->addWidget(openProjectButton);
	}

	layout->addSpacing(50);

	{
		QVBoxLayout* col = new QVBoxLayout();
		layout->addLayout(col, 1);

		QLabel* recentProjectsLabel = new QLabel(QStringLiteral("Recent Projects: "), this);
		recentProjectsLabel->setObjectName(QStringLiteral("titleLabel"));
		col->addWidget(recentProjectsLabel);

		col->addSpacing(20);

		for (size_t i = 0; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount(); ++i)
		{
			QtRecentProjectButton* button = new QtRecentProjectButton(this);
			button->setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
			button->setIcon(m_projectIcon);
			button->setIconSize(QSize(30, 30));
			button->setMinimumSize(
				button->fontMetrics().boundingRect(button->text()).width() + 45, 40);
			button->setObjectName(QStringLiteral("recentButtonMissing"));
			button->minimumSizeHint();	  // force font loading
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
		parentWidget()->height() / 2 - size.height() / 2);
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
