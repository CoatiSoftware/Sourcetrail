#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"

class StatusBarView;

class StatusBarController
	: public Controller
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageStatus>
{
public:
	StatusBarController(void);
	virtual ~StatusBarController(void);

	StatusBarView* getView();

private:
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageStatus* message);

	void setStatus(const std::string& status, bool isError, bool showLoader);
};

#endif // STATUS_BAR_CONTROLLER_H
