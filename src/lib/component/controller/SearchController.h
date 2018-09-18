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
	~SearchController();

private:
	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateFullTextSearch* message);
	virtual void handleMessage(MessageActivateLegend* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageSearchAutocomplete* message);

	SearchView* getView();

	virtual void clear();

	StorageAccess* m_storageAccess;
};

#endif // SEARCH_CONTROLLER_H
