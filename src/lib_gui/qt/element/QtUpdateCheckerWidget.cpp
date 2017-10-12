#include "qt/element/QtUpdateCheckerWidget.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QPushButton>
#include <QUrl>

#include "qt/network/QtUpdateChecker.h"
#include "settings/ApplicationSettings.h"
#include "utility/TimeStamp.h"

QtUpdateCheckerWidget::QtUpdateCheckerWidget(QWidget* parent)
	: QWidget(parent)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_button = new QPushButton("check for new version");
	m_button->setObjectName("updateButton");
	m_button->setCursor(Qt::PointingHandCursor);
	layout->addWidget(m_button);

	if (appSettings->getAutomaticUpdateCheck())
	{
		if (QtUpdateChecker::needsAutomaticCheck())
		{
			checkUpdate(false);
		}
		else
		{
			Version version = appSettings->getUpdateVersion();
			QString url = QString::fromStdString(appSettings->getUpdateDownloadUrl());
			if (version > Version::getApplicationVersion() && !url.isEmpty())
			{
				setDownloadUrl(url);
			}
			else
			{
				m_button->setText("up-to-date");
				m_button->setEnabled(false);
			}
		}
	}
	else
	{
		connect(m_button, &QPushButton::clicked, [this](){ checkUpdate(true); });
	}
}

void QtUpdateCheckerWidget::checkUpdate(bool force)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	appSettings->setUpdateVersion(Version::getApplicationVersion());
	appSettings->setUpdateDownloadUrl("");
	appSettings->save();

	m_button->setText("checking for update...");
	m_button->setEnabled(false);

	QtUpdateChecker::check(force,
		[this, appSettings](QtUpdateChecker::Result result)
		{
			if (!result.success)
			{
				m_button->setText("update check failed");
			}
			else if (result.url.isEmpty())
			{
				m_button->setText("up-to-date");
			}
			else
			{
				setDownloadUrl(result.url);

				appSettings->setUpdateVersion(result.version);
				appSettings->setUpdateDownloadUrl(result.url.toStdString());
				appSettings->save();
			}
		}
	);
}

void QtUpdateCheckerWidget::setDownloadUrl(QString url)
{
	m_button->setText("new version available");
	m_button->disconnect();
	connect(m_button, &QPushButton::clicked, this,
		[url]()
		{
			QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
		}
	);
	m_button->setEnabled(true);
}
