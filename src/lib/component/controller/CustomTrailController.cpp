#include "CustomTrailController.h"

#include "CustomTrailView.h"
#include "NodeTypeSet.h"
#include "StorageAccess.h"

CustomTrailController::CustomTrailController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

void CustomTrailController::clear()
{
	getView()->clearView();
	getView()->setAvailableNodeAndEdgeTypes(
		m_storageAccess->getAvailableNodeTypes(), m_storageAccess->getAvailableEdgeTypes());
}

void CustomTrailController::autocomplete(const std::wstring query, bool from)
{
	NodeTypeSet nodeTypes = NodeTypeSet::all();
	nodeTypes.remove(NodeType(NODE_MODULE));
	nodeTypes.remove(NodeType(NODE_NAMESPACE));
	nodeTypes.remove(NodeType(NODE_PACKAGE));

	getView()->showAutocompletions(
		m_storageAccess->getAutocompletionMatches(query, nodeTypes, false), from);
}

void CustomTrailController::activateTrail(MessageActivateTrail message)
{
	Id nodeId = message.originId ? message.originId : message.targetId;
	message.searchMatches = m_storageAccess->getSearchMatchesForTokenIds({nodeId});
	message.dispatch();
}

void CustomTrailController::handleMessage(MessageCustomTrailShow* message)
{
	getView()->showView();
}

void CustomTrailController::handleMessage(MessageIndexingFinished* message)
{
	clear();
}

void CustomTrailController::handleMessage(MessageWindowClosed* message)
{
	getView()->hideView();
}

CustomTrailView* CustomTrailController::getView()
{
	return Controller::getView<CustomTrailView>();
}
