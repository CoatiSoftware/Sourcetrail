#ifndef QT_REQUEST_H
#define QT_REQUEST_H

#include <QByteArray>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class QtRequest: public QObject
{
	Q_OBJECT

public:
	QtRequest();
	void sendRequest(const QString& url);

signals:
	void receivedData(QByteArray bytes);

private slots:
	void finished(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkManager;
};

#endif	  // QT_REQUEST_H
