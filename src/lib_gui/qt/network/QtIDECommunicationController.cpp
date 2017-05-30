#include "QtIDECommunicationController.h"

#include <functional>

#include "settings/ApplicationSettings.h"

QtIDECommunicationController::QtIDECommunicationController(QObject* parent, StorageAccess* storageAccess)
	: IDECommunicationController(storageAccess)
	, m_tcpWrapper(parent)
{
	m_tcpWrapper.setReadCallback(std::bind(&QtIDECommunicationController::handleIncomingMessage, this, std::placeholders::_1));
}

QtIDECommunicationController::~QtIDECommunicationController()
{}

void QtIDECommunicationController::startListening()
{
	m_onQtThread(
		[=]()
		{
			ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
			m_tcpWrapper.setServerPort(appSettings->getSourcetrailPort());
			m_tcpWrapper.setClientPort(appSettings->getPluginPort());
			m_tcpWrapper.startListening();

			sendUpdatePing();
		}
	);

}

void QtIDECommunicationController::stopListening()
{
	m_onQtThread(
		[=]()
		{
			m_tcpWrapper.stopListening();
		}
	);
}

bool QtIDECommunicationController::isListening() const
{
	return m_tcpWrapper.isListening();
}

void QtIDECommunicationController::sendMessage(const std::string& message) const
{
	m_tcpWrapper.sendMessage(message);
}
