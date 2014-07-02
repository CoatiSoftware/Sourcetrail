#include "component/controller/GraphController.h"

#include "component/view/GraphView.h"
#include "data/access/GraphAccess.h"

#include "utility/logging/logging.h"


GraphController::GraphController(std::shared_ptr<GraphAccess> graphAccess)
	: m_graphAccess(graphAccess)
{
}

GraphController::~GraphController()
{
}

GraphView* GraphController::getView()
{
	return Controller::getView<GraphView>();
}
