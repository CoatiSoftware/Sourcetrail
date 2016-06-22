#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/messaging/type/MessageSearchFullText.h"
#include "utility/messaging/type/MessageShowErrors.h"

class StorageAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageSearchAutocomplete>
	, public MessageListener<MessageSearchFullText>
	, public MessageListener<MessageShowErrors>
{
public:
	SearchController(StorageAccess* storageAccess);
	~SearchController();

private:
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageSearchAutocomplete* message);
	virtual void handleMessage(MessageSearchFullText* message);
	virtual void handleMessage(MessageShowErrors* message);

	SearchView* getView();

	virtual void clear();

	StorageAccess* m_storageAccess;
};

#endif // SEARCH_CONTROLLER_H
