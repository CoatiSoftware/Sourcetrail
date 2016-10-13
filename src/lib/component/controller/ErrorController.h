#ifndef ERROR_CONTROLLER_H
#define ERROR_CONTROLLER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageNewErrors.h"
#include "utility/messaging/type/MessageShowErrors.h"

#include "component/controller/Controller.h"
#include "component/view/ErrorView.h"

class StorageAccess;

class ErrorController
	: public Controller
	, public MessageListener<MessageClearErrorCount>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageNewErrors>
	, public MessageListener<MessageShowErrors>
{
public:
	ErrorController(StorageAccess* storageAccess);
	~ErrorController();

private:
	virtual void handleMessage(MessageClearErrorCount* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageNewErrors* message);
	virtual void handleMessage(MessageShowErrors* message);

	ErrorView* getView() const;

	virtual void clear();

	StorageAccess* m_storageAccess;
};

#endif // ERROR_CONTROLLER_H
