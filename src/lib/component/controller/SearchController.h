#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/error/MessageActivateErrors.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"
#include "utility/messaging/type/MessageSearchFullText.h"

class StorageAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageSearchAutocomplete>
	, public MessageListener<MessageSearchFullText>
{
public:
	SearchController(StorageAccess* storageAccess);
	~SearchController();

private:
	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageSearchAutocomplete* message);
	virtual void handleMessage(MessageSearchFullText* message);

	SearchView* getView();

	virtual void clear();

	StorageAccess* m_storageAccess;
};

#endif // SEARCH_CONTROLLER_H
