#ifndef QT_COREAPPLICTION_H
#define QT_COREAPPLICTION_H

#include <QCoreApplication>
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageQuitApplication.h"
#include "utility/messaging/type/MessageStatus.h"

class License;
class UnixSignalWatcher;

class QtCoreApplication
	: public QCoreApplication
	, public MessageListener<MessageQuitApplication>
	, public MessageListener<MessageStatus>
{
	Q_OBJECT
public:
	QtCoreApplication(int argc, char **argv);
	virtual ~QtCoreApplication();
	bool saveLicense(License license);
signals:
	void quitSignal();
private:
	virtual void handleMessage(MessageQuitApplication* message);
	virtual void handleMessage(MessageStatus* message);
};


#endif // QT_COREAPPLICATION

