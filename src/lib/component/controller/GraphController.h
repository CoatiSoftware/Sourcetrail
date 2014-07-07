#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"

#include "component/controller/Controller.h"

class GraphView;
class GraphAccess;

class GraphController:
	public Controller,
	public MessageListener<MessageActivateToken>
{
public:
	GraphController(std::shared_ptr<GraphAccess> graphAccess);
	~GraphController();

private:
	virtual void handleMessage(MessageActivateToken* message);
	GraphView* getView();

	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // GRAPH_CONTROLLER_H
