#include "QtIDECommunicationController.h"

#include <functional>

QtIDECommunicationController::QtIDECommunicationController(QObject* parent, StorageAccess* storageAccess)
	: IDECommunicationController(storageAccess)
	, m_tcpWrapper(parent)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	m_tcpWrapper.setReadCallback(std::bind(&QtIDECommunicationController::handleIncomingMessage, this, std::placeholders::_1));
	m_tcpWrapper.setServerPort(appSettings->getCoatiPort());
	m_tcpWrapper.setClientPort(appSettings->getPluginPort());
	m_tcpWrapper.startListening();
}

QtIDECommunicationController::~QtIDECommunicationController()
{}

bool QtIDECommunicationController::isListening() const
{
	return m_tcpWrapper.isListening();
}

void QtIDECommunicationController::sendMessage(const std::string& message) const
{
	m_tcpWrapper.sendMessage(message);
}