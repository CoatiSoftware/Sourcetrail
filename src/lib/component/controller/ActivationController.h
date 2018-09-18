#ifndef ACTIVATION_CONTROLLER_H
#define ACTIVATION_CONTROLLER_H

#include <memory>

#include "Controller.h"

#include "MessageListener.h"
#include "MessageActivateEdge.h"
#include "MessageActivateFile.h"
#include "MessageActivateNodes.h"
#include "MessageActivateTokenIds.h"
#include "MessageActivateSourceLocations.h"
#include "MessageResetZoom.h"
#include "MessageSearch.h"
#include "MessageZoom.h"

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
	virtual void handleMessage(MessageZoom* message);

	StorageAccess* m_storageAccess;
};

#endif // ACTIVATION_CONTROLLER_H
