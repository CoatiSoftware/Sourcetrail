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
	if (message->type == Edge::EDGE_AGGREGATION)
	{
		const Id sourceId = m_storageAccess->getIdForNodeWithNameHierarchy(message->fromNameHierarchy);
		const Id targetId = m_storageAccess->getIdForNodeWithNameHierarchy(message->toNameHierarchy);

		MessageActivateTokens m(m_storageAccess->getTokenIdsForAggregationEdge(sourceId, targetId));
		m.isAggregation = true;
		m.undoRedoType = message->undoRedoType;
		m.dispatchImmediately();
	}
	else
	{
		Id edgeId = message->tokenId;

		if (!message->isFresh())
		{
			edgeId = m_storageAccess->getIdForEdge(message->type, message->fromNameHierarchy, message->toNameHierarchy);
		}

		if (!edgeId)
		{
			return;
		}

		MessageActivateTokens msg(std::vector<Id>(1, edgeId));
		msg.isEdge = true;
		msg.undoRedoType = message->undoRedoType;
		msg.dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateFile* message)
{
	MessageActivateTokens m(std::vector<Id>(1, m_storageAccess->getTokenIdForFileNode(message->filePath)));
	m.undoRedoType = message->undoRedoType;
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageActivateNodes* message)
{
	std::vector<Id> nodeIds;
	if (message->isFresh())
	{
		for (const MessageActivateNodes::ActiveNode& node : message->nodes)
		{
			nodeIds.push_back(node.nodeId);
		}
	}
	else
	{
		for (const MessageActivateNodes::ActiveNode& node : message->nodes)
		{
			Id nodeId = m_storageAccess->getIdForNodeWithNameHierarchy(node.nameHierarchy);
			if (nodeId > 0)
			{
				nodeIds.push_back(nodeId);
			}
		}
	}

	MessageActivateTokens m(nodeIds);
	m.isFromSystem = message->isFromSystem;
	m.undoRedoType = message->undoRedoType;
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageActivateTokenLocations* message)
{
	std::vector<Id> nodeIds = m_storageAccess->getNodeIdsForLocationIds(message->locationIds);
	nodeIds = m_storageAccess->getActiveTokenIdsForTokenIds(nodeIds);

	MessageActivateNodes msg;
	for (Id nodeId : nodeIds)
	{
		msg.addNode(
			nodeId,
			m_storageAccess->getNodeTypeForNodeWithId(nodeId),
			m_storageAccess->getNameHierarchyForNodeWithId(nodeId)
		);
	}
	msg.dispatchImmediately();
}

void FeatureController::handleMessage(MessageSearch* message)
{
	std::vector<Id> tokenIds = m_storageAccess->getTokenIdsForMatches(message->getMatches());
	tokenIds = m_storageAccess->getActiveTokenIdsForTokenIds(tokenIds);

	MessageActivateTokens m(tokenIds);
	m.undoRedoType = message->undoRedoType;
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageSwitchColorScheme* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setColorSchemePath(message->colorSchemeFilePath);
	settings->save();

	MessageRefresh().refreshUiOnly().dispatch();
}

void FeatureController::handleMessage(MessageZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setFontSize(std::max(settings->getFontSize() + (message->zoomIn ? 1 : -1), 5));
	settings->save();

	MessageRefresh().refreshUiOnly().dispatch();
}
