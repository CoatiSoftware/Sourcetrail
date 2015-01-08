#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageSearchAutocomplete.h"

class GraphAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageSearchAutocomplete>
{
public:
	SearchController(GraphAccess* graphAccess);
	~SearchController();

private:
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageSearchAutocomplete* message);

	SearchView* getView();

	GraphAccess* m_graphAccess;

	bool m_ignoreNextMessageActivateTokens;
};

#endif // SEARCH_CONTROLLER_H
