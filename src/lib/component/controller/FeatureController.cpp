#include "component/controller/FeatureController.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageStatus.h"

FeatureController::FeatureController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

FeatureController::~FeatureController()
{
}

void FeatureController::clear()
{
}

void FeatureController::handleMessage(MessageActivateEdge* message)
{
	if (message->isAggregation())
	{
		// TODO: validate aggregationIds
		MessageActivateTokens m(message, message->aggregationIds);
		m.setKeepContent(false);
		m.isAggregation = true;
		m.dispatchImmediately();
	}
	else
	{
		Id edgeId = message->tokenId;

		if (message->isReplayed())
		{
			edgeId = m_storageAccess->getIdForEdge(message->type, message->fromNameHierarchy, message->toNameHierarchy);
		}

		std::vector<Id> ids;
		if (edgeId)
		{
			ids.push_back(edgeId);
		}

		MessageActivateTokens m(message, ids);
		m.isEdge = true;
		m.unknownNames = std::vector<std::string>(1, message->getFullName());
		m.dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateFile* message)
{
	Id fileId = m_storageAccess->getTokenIdForFileNode(message->filePath);

	if (fileId)
	{
		MessageActivateTokens(message, std::vector<Id>(1, fileId)).dispatchImmediately();
	}
	else
	{
		MessageChangeFileView msg(
			message->filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			true,
			true
		);
		msg.setIsReplayed(message->isReplayed());
		msg.setKeepContent(message->keepContent());
		msg.dispatchImmediately();
	}
}

void FeatureController::handleMessage(MessageActivateNodes* message)
{
	std::vector<Id> nodeIds;
	if (!message->isReplayed())
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


	MessageActivateTokens m(message, nodeIds);
	for (const MessageActivateNodes::ActiveNode& node : message->nodes)
	{
		m.unknownNames.push_back(node.nameHierarchy.getQualifiedName());
	}
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageSearch* message)
{
	const std::vector<SearchMatch>& matches = message->getMatches();

	for (const SearchMatch& match : matches)
	{
		if (match.searchType == SearchMatch::SEARCH_COMMAND &&
			match.getFullName() == SearchMatch::getCommandName(SearchMatch::COMMAND_ALL))
		{
			MessageActivateAll msg;
			msg.setIsReplayed(message->isReplayed());
			msg.dispatchImmediately();
			return;
		}
		else if (match.searchType == SearchMatch::SEARCH_COMMAND &&
			match.getFullName() == SearchMatch::getCommandName(SearchMatch::COMMAND_ERROR))
		{
			MessageShowErrors msg(ErrorCountInfo(-1, 0));
			msg.setIsReplayed(message->isReplayed());
			msg.dispatchImmediately();
			return;
		}
	}

	std::vector<Id> tokenIds = m_storageAccess->getTokenIdsForMatches(matches);

	MessageActivateTokens m(message, tokenIds);
	for (const SearchMatch& match : matches)
	{
		m.unknownNames.push_back(match.text);
	}
	if (!message->isReplayed())
	{
		m.isFromSearch = true;
	}
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageActivateTokenIds* message)
{
	MessageActivateTokens(message, message->tokenIds).dispatchImmediately();
}

void FeatureController::handleMessage(MessageActivateTokenLocations* message)
{
	std::vector<Id> nodeIds = m_storageAccess->getNodeIdsForLocationIds(message->locationIds);
	MessageActivateNodes m;
	for (Id nodeId : nodeIds)
	{
		m.addNode(
			nodeId,
			m_storageAccess->getNodeTypeForNodeWithId(nodeId),
			m_storageAccess->getNameHierarchyForNodeWithId(nodeId)
		);
	}
	m.dispatchImmediately();
}

void FeatureController::handleMessage(MessageResetZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	int fontSizeStd = settings->getFontSizeStd();

	if (settings->getFontSize() != fontSizeStd)
	{
		settings->setFontSize(fontSizeStd);
		settings->save();

		MessageRefresh().refreshUiOnly().dispatch();
	}

	MessageStatus("Zoom: 100%").dispatch();
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
	bool zoomIn = message->zoomIn;

	ApplicationSettings* settings = ApplicationSettings::getInstance().get();

	int fontSize = settings->getFontSize();
	int standardSize = settings->getFontSizeStd();
	int maxSize = settings->getFontSizeMax();
	int minSize = settings->getFontSizeMin();

	if ((fontSize >= maxSize && zoomIn)
		|| (fontSize <= minSize && !zoomIn))
	{
		return;
	}

	settings->setFontSize(fontSize + (message->zoomIn ? 1 : -1));
	settings->save();

	fontSize = settings->getFontSize();
	int zoom = (fontSize * 100) / standardSize;

	std::stringstream text;
	text << "Zoom: " << zoom << "%";
	MessageStatus(text.str()).dispatch();

	MessageRefresh().refreshUiOnly().dispatch();
}

