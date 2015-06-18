#include "component/controller/FeatureController.h"

#include "utility/messaging/type/MessageActivateTokens.h"

#include "data/access/StorageAccess.h"

FeatureController::FeatureController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

FeatureController::~FeatureController()
{
}

void FeatureController::handleMessage(MessageActivateEdge* message)
{
	Id edgeId = message->tokenId;

	if (!message->isFresh())
	{
		edgeId = m_storageAccess->getIdForEdgeWithName(message->name);
	}

	if (!edgeId)
	{
		return;
	}

	if (message->type == Edge::EDGE_AGGREGATION)
	{
		MessageActivateTokens(m_storageAccess->getTokenIdsForAggregationEdge(edgeId)).dispatch();
		return;
	}

	MessageActivateTokens msg(std::vector<Id>(1, edgeId));
	msg.isEdge = true;
	msg.dispatch();
}

void FeatureController::handleMessage(MessageActivateFile* message)
{
	MessageActivateTokens(std::vector<Id>(1, m_storageAccess->getTokenIdForFileNode(message->filePath))).dispatch();
}

void FeatureController::handleMessage(MessageActivateNode* message)
{
	Id nodeId = message->tokenId;

	if (!message->isFresh())
	{
		nodeId = m_storageAccess->getIdForNodeWithName(message->name);
	}

	if (nodeId)
	{
		MessageActivateTokens msg(nodeId);
		msg.isFromSystem = message->isFromSystem;
		msg.dispatch();
	}
}

void FeatureController::handleMessage(MessageActivateTokenLocation* message)
{
	if (message->locationId)
	{
		Id nodeId = m_storageAccess->getActiveNodeIdForLocationId(message->locationId);
		MessageActivateNode(
			nodeId,
			m_storageAccess->getNodeTypeForNodeWithId(nodeId),
			m_storageAccess->getNameForNodeWithId(nodeId)
		).dispatch();
	}
}

void FeatureController::handleMessage(MessageSearch* message)
{
	MessageActivateTokens(m_storageAccess->getTokenIdsForQuery(message->getQuery())).dispatch();
}
