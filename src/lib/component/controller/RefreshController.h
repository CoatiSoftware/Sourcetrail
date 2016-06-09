#ifndef REFRESH_CONTROLLER_H
#define REFRESH_CONTROLLER_H

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageAutoRefreshChanged.h"
#include "utility/messaging/type/MessageWindowFocus.h"

class RefreshView;

class RefreshController
	: public Controller
	, public MessageListener<MessageAutoRefreshChanged>
	, public MessageListener<MessageWindowFocus>
{
public:
	RefreshController();
	virtual ~RefreshController();

	virtual void clear();

private:
	virtual void handleMessage(MessageAutoRefreshChanged* message);
	virtual void handleMessage(MessageWindowFocus* message);

	RefreshView* getView();

	bool m_autoRefreshEnabled;
};

#endif // REFRESH_CONTROLLER_H
