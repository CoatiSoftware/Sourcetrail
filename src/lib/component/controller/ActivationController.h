#ifndef ACTIVATION_CONTROLLER_H
#define ACTIVATION_CONTROLLER_H

#include <memory>

#include "Controller.h"

#include "MessageActivateEdge.h"
#include "MessageActivateFile.h"
#include "MessageActivateNodes.h"
#include "MessageActivateSourceLocations.h"
#include "MessageActivateTokenIds.h"
#include "MessageListener.h"
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

	void clear() override;

private:
	void handleMessage(MessageActivateEdge* message) override;
	void handleMessage(MessageActivateFile* message) override;
	void handleMessage(MessageActivateNodes* message) override;
	void handleMessage(MessageActivateTokenIds* message) override;
	void handleMessage(MessageActivateSourceLocations* message) override;
	void handleMessage(MessageResetZoom* message) override;
	void handleMessage(MessageSearch* message) override;
	void handleMessage(MessageZoom* message) override;

	StorageAccess* m_storageAccess;
};

#endif	  // ACTIVATION_CONTROLLER_H
