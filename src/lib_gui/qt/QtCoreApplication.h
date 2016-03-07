#ifndef QT_COREAPPLICTION_H
#define QT_COREAPPLICTION_H

#include <QCoreApplication>
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

class License;

class QtCoreApplication
	: public QCoreApplication
	, public MessageListener<MessageFinishedParsing>
{
public:
	QtCoreApplication(int argc, char **argv);
	virtual ~QtCoreApplication();
	bool saveLicense(License license);
signals:
	void quitSignal();
private:
	virtual void handleMessage(MessageFinishedParsing* message);
};


#endif // QT_COREAPPLICATION

