#include "QtTcpWrapper.h"
#include <qdatastream.h>

#include "logging.h"

QtTcpWrapper::QtTcpWrapper(
	QObject* parent, const std::string& ip, const quint16 serverPort, const quint16 clientPort)
	: QObject(parent), m_serverPort(serverPort), m_clientPort(clientPort), m_ip(ip)
{
	m_tcpServer = new QTcpServer(this);

	connect(m_tcpServer, &QTcpServer::newConnection, this, &QtTcpWrapper::acceptConnection);
}

void QtTcpWrapper::startListening()
{
	if (!m_tcpServer->listen(QHostAddress::LocalHost, m_serverPort))
	{
		LOG_ERROR_STREAM(
			<< "TCP server failed to start with error: \"" + m_tcpServer->errorString().toStdString() +
				"\". Unable to listen for IDE plugin messages.");
	}
}

void QtTcpWrapper::stopListening()
{
	if (isListening())
	{
		m_tcpServer->close();
	}
}

QtTcpWrapper::~QtTcpWrapper()
{
	if (m_tcpServer != nullptr)
	{
		if (m_tcpServer->isListening())
		{
			m_tcpServer->close();
		}

		delete m_tcpServer;
	}
}

void QtTcpWrapper::sendMessage(const std::wstring& message) const
{
	QByteArray data = QString::fromStdWString(message).toUtf8();

	QTcpSocket socket;
	socket.connectToHost(QHostAddress::LocalHost, m_clientPort);

	if (socket.waitForConnected())
	{
		socket.write(data);
		socket.flush();
		socket.waitForBytesWritten();
		socket.close();
	}
}

void QtTcpWrapper::setReadCallback(const std::function<void(const std::wstring&)>& callback)
{
	m_readCallback = callback;
}

quint16 QtTcpWrapper::getServerPort() const
{
	return m_serverPort;
}

void QtTcpWrapper::setServerPort(const quint16 serverPort)
{
	m_serverPort = serverPort;
}

quint16 QtTcpWrapper::getClientPort() const
{
	return m_clientPort;
}

void QtTcpWrapper::setClientPort(const quint16 clientPort)
{
	m_clientPort = clientPort;
}

bool QtTcpWrapper::isListening() const
{
	return m_tcpServer->isListening();
}

void QtTcpWrapper::acceptConnection()
{
	m_tcpClient = m_tcpServer->nextPendingConnection();

	connect(m_tcpClient, &QTcpSocket::readyRead, this, &QtTcpWrapper::startRead);
}

void QtTcpWrapper::startRead()
{
	QByteArray byteArray;
	while (m_tcpClient->atEnd() == false)
	{
		QByteArray data = m_tcpClient->read(128);
		byteArray += data;
	}

	/*QDataStream stream(&byteArray, QIODevice::ReadOnly);
	QString string;
	stream >> string;*/

	QString message = QString::fromUtf8(byteArray);

	if (m_readCallback != nullptr)
	{
		m_readCallback(message.toStdWString());
	}

	/*char buffer[1024] = { 0 };
	m_tcpClient->read(buffer, m_tcpClient->bytesAvailable());

	m_tcpClient->close();

	if (m_readCallback != nullptr)
	{
		m_readCallback(buffer);
	}*/
}
