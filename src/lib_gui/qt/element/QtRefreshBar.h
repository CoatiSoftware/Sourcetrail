#ifndef QT_REFRESH_BAR_H
#define QT_REFRESH_BAR_H

#include <QFrame>

#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/MessageListener.h"

class QPushButton;

class QtRefreshBar
	: public QFrame
	, public MessageListener<MessageEnteredLicense>
{
	Q_OBJECT

public:
	QtRefreshBar();
	virtual ~QtRefreshBar();

	void refreshStyle();

private slots:
	void refreshClicked();

private:
	virtual void handleMessage(MessageEnteredLicense* message);

	QtThreadedLambdaFunctor m_onQtThread;

	QPushButton* m_refreshButton;
};

#endif // QT_REFRESH_BAR_H
