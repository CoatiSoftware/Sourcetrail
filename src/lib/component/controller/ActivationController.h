#ifndef ACTIVATION_CONTROLLER_H
#define ACTIVATION_CONTROLLER_H

#include <memory>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageActivateTokenIds.h"
#include "utility/messaging/type/MessageActivateSourceLocations.h"
#include "utility/messaging/type/MessageResetZoom.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageShowErrorsForFile.h"
#include "utility/messaging/type/MessageZoom.h"

class StorageAccess;

class ActivationController
	: public Controller
	, public MessageListener<MessageActivateEdge>
	, public MessageListener<MessageActivateFile>
	, public MessageListener<MessageActivateNodes>
	, public MessageListener<MessageActivateTokenIds>
	, public MessageListener<MessageActivateSourceLocations>
	, public MessageListener<MessageResetZoom>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageShowErrorsForFile>
	, public MessageListener<MessageZoom>
{
public:
	ActivationController(StorageAccess* storageAccess);
	~ActivationController();

	virtual void clear();

private:
	virtual void handleMessage(MessageActivateEdge* message);
	virtual void handleMessage(MessageActivateFile* message);
	virtual void handleMessage(MessageActivateNodes* message);
	virtual void handleMessage(MessageActivateTokenIds* message);
	virtual void handleMessage(MessageActivateSourceLocations* message);
	virtual void handleMessage(MessageResetZoom* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageShowErrorsForFile* message);
	virtual void handleMessage(MessageZoom* message);

	StorageAccess* m_storageAccess;
};

#endif // ACTIVATION_CONTROLLER_H
