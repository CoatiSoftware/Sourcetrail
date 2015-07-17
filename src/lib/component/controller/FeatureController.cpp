#include "component/controller/FeatureController.h"

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageRefresh.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

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
		MessageActivateTokens m(m_storageAccess->getTokenIdsForAggregationEdge(edgeId));
		m.undoRedoType = message->undoRedoType;
		m.dispatch();
		return;
	}

	MessageActivateTokens msg(std::vector<Id>(1, edgeId));
	msg.isEdge = true;
	msg.undoRedoType = message->undoRedoType;
	msg.dispatchImmediately();
}

void FeatureController::handleMessage(MessageActivateFile* message)
{
	MessageActivateTokens m(std::vector<Id>(1, m_storageAccess->getTokenIdForFileNode(message->filePath)));
	m.undoRedoType = message->undoRedoType;
	m.dispatchImmediately();
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
		MessageActivateTokens m(nodeId);
		m.isFromSystem = message->isFromSystem;
		m.undoRedoType = message->undoRedoType;
		m.dispatchImmediately();
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
		).dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageSearch* message)
{
	MessageActivateTokens m(m_storageAccess->getTokenIdsForQuery(message->getQuery()));
	m.undoRedoType = message->undoRedoType;
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageSwitchColorScheme* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setColorSchemePath(message->colorSchemeFilePath);
	settings->save();

	MessageRefresh(true).dispatch();
}

void FeatureController::handleMessage(MessageZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setFontSize(std::max(settings->getFontSize() + (message->zoomIn ? 1 : -1), 5));
	settings->save();

	MessageRefresh(true).dispatch();
}
