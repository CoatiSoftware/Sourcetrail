#include "QtIDECommunicationController.h"

#include <functional>

QtIDECommunicationController::QtIDECommunicationController(QObject* parent, StorageAccess* storageAccess)
	: IDECommunicationController(storageAccess)
	, m_tcpWrapper(parent)
{
	m_tcpWrapper.setReadCallback(std::bind(&QtIDECommunicationController::handleIncomingMessage, this, std::placeholders::_1));
}

QtIDECommunicationController::~QtIDECommunicationController()
{}

void QtIDECommunicationController::sendMessage(const std::string& message) const
{
	m_tcpWrapper.sendMessage(message);
}