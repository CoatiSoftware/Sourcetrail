#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageRefresh.h"

class GraphAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateToken>
	, public MessageListener<MessageFind>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageRefresh>
{
public:
	SearchController(GraphAccess* graphAccess);
	~SearchController();

	void search(const std::string& s);
	//void autocomplete(const std::string& s);

private:
	virtual void handleMessage(MessageActivateToken* message);
	virtual void handleMessage(MessageFind* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageRefresh* message);
	SearchView* getView();

	GraphAccess* m_graphAccess;
};

#endif // SEARCH_CONTROLLER_H
