#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

class GraphAccess;
class SearchView;

class SearchController
	: public Controller
	, public MessageListener<MessageActivateToken>
	, public MessageListener<MessageFinishedParsing>
{
public:
	SearchController(std::shared_ptr<GraphAccess> graphAccess);
	~SearchController();

	void search(const std::string& s);
	//void autocomplete(const std::string& s);

private:
	virtual void handleMessage(MessageActivateToken* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	SearchView* getView();

	std::shared_ptr<GraphAccess> m_graphAccess;
};

#endif // SEARCH_CONTROLLER_H
