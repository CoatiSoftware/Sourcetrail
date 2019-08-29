#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "ActivationListener.h"
#include "Controller.h"
#include "MessageListener.h"
#include "MessageFind.h"
#include "MessageSearchAutocomplete.h"

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

#endif // SEARCH_CONTROLLER_H
