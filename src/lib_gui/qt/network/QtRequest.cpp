#include "qt/network/QtRequest.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "utility/logging/logging.h"

QtRequest::QtRequest()
{
	m_networkManager = new QNetworkAccessManager(this);
	QObject::connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

void QtRequest::sendRequest(QString url)
{
	LOG_INFO_STREAM(<< "send HTTP request: " << url.toStdString());

	QNetworkRequest request;
	request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
	request.setUrl(QUrl(url));
	m_networkManager->get(request);
}

void QtRequest::finished(QNetworkReply *reply)
{
	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

	Q_UNUSED(statusCodeV);
	Q_UNUSED(redirectionTargetUrl);

	if (reply->error() != QNetworkReply::NoError)
	{
		LOG_ERROR_STREAM(<< "An error occured during http request. ERRORCODE: " << reply->error());
	}

	QByteArray bytes = reply->readAll();
	LOG_INFO_STREAM(<< "received HTTP reply: " << bytes.toStdString());

	delete reply;

	emit receivedData(bytes);
}
