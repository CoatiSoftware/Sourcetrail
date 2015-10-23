#ifndef QT_IDE_COMMUNICATION_CONTROLLER
#define QT_IDE_COMMUNICATION_CONTROLLER

#include <qobject.h>

#include "QtTcpWrapper.h"

#include "component/controller/IDECommunicationController.h"

class StorageAccess;

class QtIDECommunicationController
	: public IDECommunicationController
{
public:
	QtIDECommunicationController(QObject* parent, StorageAccess* storageAccess);
	~QtIDECommunicationController();

private:
	virtual void sendMessage(const std::string& message) const;

	QtTcpWrapper m_tcpWrapper;
};

#endif // QT_IDE_COMMUNICATION_CONTROLLER