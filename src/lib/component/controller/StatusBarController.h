#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageError.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageLoadSource.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"

class StatusBarView;

class StatusBarController
	: public Controller
	, public MessageListener<MessageError>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageLoadSource>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageStatus>
{
public:
	StatusBarController(void);
	virtual ~StatusBarController(void);

	StatusBarView* getView();

private:
	virtual void handleMessage(MessageError* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageLoadSource* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageStatus* message);

	void setStatus(const std::string& status, bool isError = false);
};

#endif // STATUS_BAR_CONTROLLER_H
