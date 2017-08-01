#include "SocketTest.h"

#include "utility/logging/logging.h"

SocketTest::SocketTest(QObject* parent)
	: QObject(parent)
	, m_socket(NULL)
{
	m_socket = new QUdpSocket(this);
	QHostAddress address("127.0.0.1");
	m_socket->bind(address, 6666);

	connect(m_socket, &QUdpSocket::readyRead, this, &SocketTest::readyRead);
}

SocketTest::~SocketTest()
{
	if(m_socket != NULL)
	{
		delete m_socket;
	}
}

void SocketTest::sendTestMessage()
{
	LOG_WARNING("SocketTest::sendTestMessage");

	QByteArray Data;
    Data.append("qt foo");

	m_socket->writeDatagram(Data, QHostAddress::Any, 6666);
}

void SocketTest::readyRead()
{
	LOG_WARNING("SocketTest::readyRead");

	QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());
    
    QHostAddress sender;
    quint16 senderPort;

	m_socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

 	std::string message = buffer.data();

	LOG_WARNING(message);
}
