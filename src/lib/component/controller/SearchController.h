#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "helper/ActivationListener.h"
#include "Controller.h"
#include "../../utility/messaging/type/search/MessageFind.h"
#include "../../utility/messaging/MessageListener.h"
#include "../../utility/messaging/type/search/MessageSearchAutocomplete.h"

class StorageAccess;
class SearchView;

class SearchController
	: public Controller
	, public ActivationListener
	, public MessageListener<MessageFind>
	, public MessageListener<MessageSearchAutocomplete>
{
public:
	SearchController(StorageAccess* storageAccess);
	~SearchController() = default;

	Id getSchedulerId() const override;

private:
	void handleActivation(const MessageActivateBase* message) override;

	void handleMessage(MessageFind* message) override;
	void handleMessage(MessageSearchAutocomplete* message) override;

	SearchView* getView();

	void clear() override;

	void updateMatches(const MessageActivateBase* message, bool updateView = true);

	StorageAccess* m_storageAccess;
};

#endif	  // SEARCH_CONTROLLER_H
