#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"

class StorageAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageSearchAutocomplete>
{
public:
	SearchController(StorageAccess* storageAccess);
	~SearchController();

private:
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageSearchAutocomplete* message);

	SearchView* getView();

	StorageAccess* m_storageAccess;

	bool m_ignoreNextMessageActivateTokens;
};

#endif // SEARCH_CONTROLLER_H
