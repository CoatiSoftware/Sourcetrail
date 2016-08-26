#ifndef QT_IDE_COMMUNICATION_CONTROLLER
#define QT_IDE_COMMUNICATION_CONTROLLER

#include <qobject.h>

#include "QtTcpWrapper.h"

#include "component/controller/IDECommunicationController.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "settings/ApplicationSettings.h"

class StorageAccess;

class QtIDECommunicationController
	: public IDECommunicationController
{
public:
	QtIDECommunicationController(QObject* parent, StorageAccess* storageAccess);
	~QtIDECommunicationController();

	virtual void startListening();
	virtual void stopListening();

	virtual bool isListening() const;

private:
	virtual void sendMessage(const std::string& message) const;

	QtTcpWrapper m_tcpWrapper;

	QtThreadedLambdaFunctor m_onQtThread;
};

#endif // QT_IDE_COMMUNICATION_CONTROLLER
