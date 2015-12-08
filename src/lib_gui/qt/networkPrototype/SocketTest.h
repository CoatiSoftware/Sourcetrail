#ifndef SOCKET_TEST_H
#define SOCKET_TEST_H

#include <qobject.h>
#include <qudpsocket.h>

class SocketTest : public QObject
{
	Q_OBJECT

public:
	SocketTest(QObject* parent);
	~SocketTest();

	void sendTestMessage();

signals:

public slots:
	void readyRead();

private:
	QUdpSocket* m_socket;
};

#endif // SOCKET_TEST_H