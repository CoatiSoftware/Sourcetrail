#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageError.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"

class StatusBarView;

class StatusBarController
    : public Controller
    , public MessageListener<MessageError>
    , public MessageListener<MessageFinishedParsing>
    , public MessageListener<MessageStatus>
{
public:

	StatusBarController(void);
	~StatusBarController(void);

	StatusBarView* getView();
private:
    void setStatus(const std::string& status, bool isError = false);

    virtual void handleMessage(MessageError* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageStatus* message);
};

#endif // STATUS_BAR_CONTROLLER_H
