#ifndef CUSTOM_TRAIL_CONTROLLER_H
#define CUSTOM_TRAIL_CONTROLLER_H

#include "Controller.h"
#include "MessageActivateTrail.h"
#include "MessageCustomTrailShow.h"
#include "MessageIndexingFinished.h"
#include "MessageListener.h"
#include "MessageWindowClosed.h"

class CustomTrailView;
class StorageAccess;

class CustomTrailController
	: public Controller
	, public MessageListener<MessageCustomTrailShow>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageWindowClosed>
{
public:
	CustomTrailController(StorageAccess* storageAccess);
	~CustomTrailController() = default;

	// Controller implementation
	void clear() override;

	void autocomplete(const std::wstring query, bool from);
	void activateTrail(MessageActivateTrail message);

private:
	void handleMessage(MessageCustomTrailShow* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageWindowClosed* message) override;

	CustomTrailView* getView();

	StorageAccess* m_storageAccess;
};

#endif	  // CUSTOM_TRAIL_CONTROLLER_H
