#ifndef ACTIVATION_CONTROLLER_H
#define ACTIVATION_CONTROLLER_H

#include <memory>

#include "Controller.h"

#include "../../utility/messaging/type/graph/MessageActivateEdge.h"
#include "../../utility/messaging/type/code/MessageActivateFile.h"
#include "../../utility/messaging/type/graph/MessageActivateNodes.h"
#include "../../utility/messaging/type/code/MessageActivateSourceLocations.h"
#include "../../utility/messaging/type/code/MessageActivateTokenIds.h"
#include "../../utility/messaging/MessageListener.h"
#include "../../utility/messaging/type/MessageResetZoom.h"
#include "../../utility/messaging/type/search/MessageSearch.h"
#include "../../utility/messaging/type/MessageZoom.h"

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
