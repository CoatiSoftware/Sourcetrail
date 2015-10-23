#ifndef QT_SOCKET_WRAPPER_H
#define QT_SOCKET_WRAPPER_H

#include <functional>
#include <qobject.h>
#include <qudpsocket.h>

#include <qtcpserver.h>
#include <qtcpsocket.h>

class QtTcpWrapper : public QObject
{
	Q_OBJECT

public:
	QtTcpWrapper(QObject* parent, const std::string& ip = "127.0.0.1", const quint16 port = 6667);
	~QtTcpWrapper();

	void sendMessage(const std::string& message) const;

	void setReadCallback(const std::function<void(const std::string&)>& callback);

signals:

public slots:
	void acceptConnection();
	void startRead();

private:
	quint16 m_port;
	std::string m_ip;

	std::function<void(const std::string&)> m_readCallback;

	QTcpServer* m_tcpServer;
	QTcpSocket* m_tcpClient;
};

#endif // QT_SOCKET_WRAPPER_H