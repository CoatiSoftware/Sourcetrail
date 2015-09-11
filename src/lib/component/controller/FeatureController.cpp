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
		const std::string sourceStartDelimiter = ":";
		const std::string sourceEndDelimiter = "->";

		const int sourceStartPosition = message->name.find(sourceStartDelimiter) + sourceStartDelimiter.size();
		const int sourceEndPosition = message->name.find(sourceEndDelimiter);
		const int targetStartPosition = sourceEndPosition + sourceEndDelimiter.size();
		const int targetEndPosition = message->name.size();

		const std::string sourceName = message->name.substr(sourceStartPosition, sourceEndPosition - sourceStartPosition);
		const std::string targetName = message->name.substr(targetStartPosition, targetEndPosition - targetStartPosition);

		const int sourceId = m_storageAccess->getIdForNodeWithName(sourceName);
		const int targetId = m_storageAccess->getIdForNodeWithName(targetName);

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
			edgeId = m_storageAccess->getIdForEdgeWithName(message->name);
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
			nodeIds.push_back(m_storageAccess->getIdForNodeWithName(node.name));
		}
	}

	if (nodeIds.size())
	{
		MessageActivateTokens m(nodeIds);
		m.isFromSystem = message->isFromSystem;
		m.undoRedoType = message->undoRedoType;
		m.dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateTokenLocations* message)
{
	std::vector<Id> nodeIds;
	MessageActivateNodes msg;

	for (Id locationId : message->locationIds)
	{
		Id nodeId = m_storageAccess->getActiveNodeIdForLocationId(locationId);
		msg.addNode(
			nodeId,
			m_storageAccess->getNodeTypeForNodeWithId(nodeId),
			m_storageAccess->getNameForNodeWithId(nodeId)
		);
	}
	msg.dispatchImmediately();
}

void FeatureController::handleMessage(MessageSearch* message)
{
	MessageActivateTokens m(m_storageAccess->getTokenIdsForMatches(message->getMatches()));
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
