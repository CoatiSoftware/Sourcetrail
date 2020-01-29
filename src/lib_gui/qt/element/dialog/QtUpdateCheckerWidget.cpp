#include "QtUpdateCheckerWidget.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QUrl>

#include "ApplicationSettings.h"
#include "QtUpdateChecker.h"
#include "TimeStamp.h"

QtUpdateCheckerWidget::QtUpdateCheckerWidget(QWidget* parent)
	: QWidget(parent), m_deleteCheck(std::make_shared<bool>(false))
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_button = new QPushButton(QStringLiteral("check for new version"));
	m_button->setObjectName(QStringLiteral("updateButton"));
	m_button->setCursor(Qt::PointingHandCursor);
	layout->addWidget(m_button);

	if (appSettings->getAutomaticUpdateCheck())
	{
		if (QtUpdateChecker::needsAutomaticCheck())
		{
			m_button->setText(QStringLiteral("checking for update..."));
			m_button->setEnabled(false);

			std::shared_ptr<bool> deleteCheck = m_deleteCheck;

			QTimer::singleShot(250, [deleteCheck, this]() {
				if (*deleteCheck.get())
				{
					return;
				}

				checkUpdate(false);
			});
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
				m_button->setText(QStringLiteral("up-to-date"));
				m_button->setEnabled(false);
			}
		}
	}
	else
	{
		connect(m_button, &QPushButton::clicked, [this]() { checkUpdate(true); });
	}
}

QtUpdateCheckerWidget::~QtUpdateCheckerWidget()
{
	*m_deleteCheck.get() = true;
}

void QtUpdateCheckerWidget::checkUpdate(bool force)
{
	m_button->setText(QStringLiteral("checking for update..."));
	m_button->setEnabled(false);

	std::shared_ptr<bool> deleteCheck = m_deleteCheck;

	QtUpdateChecker::check(force, [deleteCheck, this](QtUpdateChecker::Result result) {
		if (*deleteCheck.get())
		{
			return;
		}

		if (!result.success)
		{
			m_button->setText(QStringLiteral("update check failed"));
		}
		else if (result.url.isEmpty())
		{
			m_button->setText(QStringLiteral("up-to-date"));
		}
		else
		{
			setDownloadUrl(result.url);
		}

		emit updateReceived();
	});
}

void QtUpdateCheckerWidget::setDownloadUrl(QString url)
{
	m_button->setText(QStringLiteral("new version available"));
	m_button->disconnect();
	connect(m_button, &QPushButton::clicked, this, [url]() {
		QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
	});
	m_button->setEnabled(true);
}
