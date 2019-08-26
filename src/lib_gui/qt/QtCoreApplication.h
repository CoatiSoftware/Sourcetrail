#ifndef QT_COREAPPLICTION_H
#define QT_COREAPPLICTION_H

#include <QCoreApplication>

#include "MessageIndexingStatus.h"
#include "MessageListener.h"
#include "MessageQuitApplication.h"
#include "MessageStatus.h"

class QtCoreApplication
	: public QCoreApplication
	, public MessageListener<MessageQuitApplication>
	, public MessageListener<MessageIndexingStatus>
	, public MessageListener<MessageStatus>
{
	Q_OBJECT

public:
	QtCoreApplication(int argc, char **argv);
	virtual ~QtCoreApplication() = default;

private:
	virtual void handleMessage(MessageQuitApplication* message);
	virtual void handleMessage(MessageIndexingStatus* message);
	virtual void handleMessage(MessageStatus* message);
};

#endif // QT_COREAPPLICATION
