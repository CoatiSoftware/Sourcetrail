#include "QtNewsWidget.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QStyle>

#include "ApplicationSettings.h"
#include "logging.h"
#include "QtTextEdit.h"
#include "TimeStamp.h"
#include "utilityApp.h"
#include "Version.h"

QtNewsWidget::QtNewsWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_text = new QtTextEdit();
	m_text->setObjectName("textField");
	m_text->setReadOnly(true);
	m_text->setTabStopWidth(8 * m_text->fontMetrics().width('9'));
	m_text->setViewportMargins(6, 4, 16, 4);
	m_text->setOpenExternalLinks(true);
	layout->addWidget(m_text);

	QString placeholder = "No news available";
	if (!ApplicationSettings::getInstance()->getAutomaticUpdateCheck())
	{
		placeholder += "\n(Enable update check to fetch)";
	}
	m_text->setPlaceholderText(placeholder);

	updateNews();
}

/*
syntax:
[
	{
		// required
		"version": 1,
		"content": "<b>Hello World!</b>",

		// optional
		"conditions":
		{
			"os": ["windows", "macOS", "linux"],
			"min_version": "2017.2.0",
			"max_version": "2018.4.2",
			"weekday": ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
		}

		// optional
		"flags":
		{
			"important": true
		}
	}
]
*/

void QtNewsWidget::updateNews()
{
	const int supportedNewsItemVersion = 1;

	resetFlags();

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	std::string newsRaw = appSettings->getUpdateNews();
	if (!newsRaw.size())
	{
		setNews("");
		return;
	}

	if (TimeStamp::now().deltaHours(appSettings->getLastUpdateCheck()) >= 120)
	{
		LOG_INFO_STREAM(<< "Ignore news string, because older than 5 days.");
		setNews("");
		return;
	}

	LOG_INFO_STREAM(<< "Process news string: " << newsRaw);

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(newsRaw).toUtf8(), &error);
	if (doc.isNull() || !doc.isArray())
	{
		LOG_ERROR_STREAM(<< "News string couldn't be parsed as JSON: " << error.errorString().toStdString()
			<< "\nJSON: " << newsRaw);
		setNews("");
		return;
	}

	size_t itemNum = 0;
	for (QJsonValueRef value : doc.array())
	{
		if (!value.isObject())
		{
			continue;
		}

		itemNum++;
		QJsonObject newsItem = value.toObject();

		int version = newsItem.value("version").toInt();
		if (version > supportedNewsItemVersion)
		{
			LOG_INFO_STREAM(<< "News item " << itemNum << " version " << version << " is not supported.");
			continue;
		}

		if (!checkConditions(newsItem.value("conditions").toObject()))
		{
			LOG_INFO_STREAM(<< "News item " << itemNum << " conditions failed.");
			continue;
		}

		processFlags(newsItem.value("flags").toObject());
		setNews(newsItem.value("content").toString());
		return;
	}

	setNews("");
}

void QtNewsWidget::resetFlags()
{
	setImportant(false);
}

void QtNewsWidget::setNews(const QString& news)
{
	m_text->setHtml(news);
}

void QtNewsWidget::setImportant(bool important)
{
	m_text->setProperty("important", important);
	m_text->style()->unpolish(m_text);
	m_text->style()->polish(m_text);
}

bool QtNewsWidget::checkConditions(const QJsonObject& conditions) const
{
	if (conditions.isEmpty())
	{
		return true;
	}

	// min_version
	{
		QString minVersionString = conditions.value("min_version").toString();
		if (!minVersionString.isEmpty())
		{
			Version minVersion = Version::fromString(minVersionString.toStdString());
			if (minVersion.isValid() && minVersion > Version::getApplicationVersion())
			{
				LOG_INFO_STREAM(<< "Failed condition min_version.");
				return false;
			}
		}
	}

	// max_version
	{
		QString maxVersionString = conditions.value("max_version").toString();
		if (!maxVersionString.isEmpty())
		{
			Version maxVersion = Version::fromString(maxVersionString.toStdString());
			if (maxVersion.isValid() && maxVersion < Version::getApplicationVersion())
			{
				LOG_INFO_STREAM(<< "Failed condition max_version.");
				return false;
			}
		}
	}

	// OS
	{
		QJsonArray osStrings = conditions.value("os").toArray();
		if (!osStrings.isEmpty())
		{
			QString osString = QString::fromStdString(utility::getOsTypeString());
			if (!osStrings.contains(QJsonValue(osString)))
			{
				LOG_INFO_STREAM(<< "Failed condition os.");
				return false;
			}
		}
	}

	// weekday
	{
		QJsonArray weekdayStrings = conditions.value("weekday").toArray();
		if (!weekdayStrings.isEmpty())
		{
			QString weekdayString = QString::fromStdString(TimeStamp::now().dayOfWeekShort());
			if (!weekdayStrings.contains(QJsonValue(weekdayString)))
			{
				LOG_INFO_STREAM(<< "Failed condition weekday.");
				return false;
			}
		}
	}

	return true;
}

void QtNewsWidget::processFlags(const QJsonObject& flags)
{
	if (flags.isEmpty())
	{
		return;
	}

	// important
	{
		bool important = flags.value("important").toBool();
		if (important)
		{
			LOG_INFO_STREAM(<< "Flag important enabled.");
			setImportant(true);
		}
	}
}
