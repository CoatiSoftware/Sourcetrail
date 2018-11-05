#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "Controller.h"
#include "MessageListener.h"
#include "MessageActivateLegend.h"
#include "MessageActivateErrors.h"
#include "MessageActivateAll.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateTokens.h"
#include "MessageFind.h"
#include "MessageSearchAutocomplete.h"

class StorageAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateLegend>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageSearchAutocomplete>
{
public:
	SearchController(StorageAccess* storageAccess);
	~SearchController() = default;

	Id getSchedulerId() const override;

private:
	void handleMessage(MessageActivateAll* message) override;
	void handleMessage(MessageActivateErrors* message) override;
	void handleMessage(MessageActivateFullTextSearch* message) override;
	void handleMessage(MessageActivateLegend* message) override;
	void handleMessage(MessageActivateTokens* message) override;
	void handleMessage(MessageFind* message) override;
	void handleMessage(MessageSearchAutocomplete* message) override;

	SearchView* getView();

	void clear() override;

	void updateMatches(MessageActivateBase* message, bool updateView = true);

	StorageAccess* m_storageAccess;
};

#endif // SEARCH_CONTROLLER_H
