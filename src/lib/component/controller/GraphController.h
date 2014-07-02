#ifndef GRAPH_CONTROLLER_H
#define GRAPH_CONTROLLER_H

#include "component/controller/Controller.h"

class GraphView;
class GraphAccess;

class GraphController: public Controller
{
public:
	GraphController(std::shared_ptr<GraphAccess> graphAccess);
	~GraphController();

private:
	GraphView* getView();

	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // GRAPH_CONTROLLER_H
