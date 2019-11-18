#include "ActivationController.h"

#include "ApplicationSettings.h"
#include "StorageAccess.h"

#include "MessageActivateLegend.h"
#include "MessageActivateOverview.h"
#include "MessageActivateTokens.h"
#include "MessageChangeFileView.h"
#include "MessageErrorsAll.h"
#include "MessageFlushUpdates.h"
#include "MessageRefreshUI.h"
#include "MessageScrollToLine.h"
#include "MessageStatus.h"
#include "MessageTooltipShow.h"
#include "utility.h"

ActivationController::ActivationController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

void ActivationController::clear() {}

void ActivationController::handleMessage(MessageActivateEdge* message)
{
	if (message->isAggregation())
	{
		MessageActivateTokens m(message);
		m.tokenIds = message->aggregationIds;
		m.setKeepContent(false);
		m.isAggregation = true;
		m.dispatchImmediately();
	}
	else
	{
		MessageActivateTokens m(message);
		m.tokenIds.push_back(message->tokenId);
		m.isEdge = true;
		m.dispatchImmediately();
	}
}

void ActivationController::handleMessage(MessageActivateFile* message)
{
	Id fileId = m_storageAccess->getNodeIdForFileNode(message->filePath);

	if (fileId)
	{
		MessageActivateTokens messageActivateTokens(message);
		messageActivateTokens.tokenIds.push_back(fileId);
		messageActivateTokens.searchMatches = m_storageAccess->getSearchMatchesForTokenIds({fileId});
		messageActivateTokens.dispatchImmediately();
	}
	else
	{
		MessageChangeFileView msg(
			message->filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			MessageChangeFileView::VIEW_CURRENT,
			CodeScrollParams::toFile(message->filePath, CodeScrollParams::Target::VISIBLE));
		msg.setSchedulerId(message->getSchedulerId());
		msg.dispatchImmediately();
	}

	if (message->line > 0)
	{
		MessageScrollToLine msg(message->filePath, message->line);
		msg.setSchedulerId(message->getSchedulerId());
		msg.dispatch();
	}
}

void ActivationController::handleMessage(MessageActivateNodes* message)
{
	MessageActivateTokens m(message);
	for (const MessageActivateNodes::ActiveNode& node: message->nodes)
	{
		Id nodeId = node.nodeId;
		if (!nodeId)
		{
			nodeId = m_storageAccess->getNodeIdForNameHierarchy(node.nameHierarchy);
		}

		if (nodeId > 0)
		{
			m.tokenIds.push_back(nodeId);
		}
	}
	m.searchMatches = m_storageAccess->getSearchMatchesForTokenIds(m.tokenIds);
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageActivateTokenIds* message)
{
	MessageActivateTokens m(message);
	m.tokenIds = message->tokenIds;
	m.searchMatches = m_storageAccess->getSearchMatchesForTokenIds(message->tokenIds);
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageActivateSourceLocations* message)
{
	MessageActivateNodes msg;
	for (Id nodeId: m_storageAccess->getNodeIdsForLocationIds(message->locationIds))
	{
		msg.addNode(nodeId);
	}

	if (message->containsUnsolvedLocations && msg.nodes.size() == 1 &&
		m_storageAccess->getNameHierarchyForNodeId(msg.nodes[0].nodeId).getQualifiedName() ==
			L"unsolved symbol")
	{
		MessageTooltipShow m(message->locationIds, {}, TOOLTIP_ORIGIN_CODE);
		m.force = true;
		m.dispatch();
		return;
	}

	msg.setSchedulerId(message->getSchedulerId());
	msg.dispatchImmediately();
}

void ActivationController::handleMessage(MessageResetZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	int fontSizeStd = settings->getFontSizeStd();

	if (settings->getFontSize() != fontSizeStd)
	{
		settings->setFontSize(fontSizeStd);
		settings->save();

		MessageRefreshUI().dispatch();
	}

	MessageStatus(L"Font size: " + std::to_wstring(fontSizeStd)).dispatch();
}

void ActivationController::handleMessage(MessageSearch* message)
{
	const std::vector<SearchMatch>& matches = message->getMatches();

	if (matches.size() && matches.back().searchType == SearchMatch::SEARCH_COMMAND)
	{
		switch (matches.back().getCommandType())
		{
		case SearchMatch::COMMAND_ALL:
		case SearchMatch::COMMAND_NODE_FILTER:
		{
			MessageActivateOverview msg(message->acceptedNodeTypes);
			msg.setSchedulerId(message->getSchedulerId());
			msg.dispatch();
			return;
		}

		case SearchMatch::COMMAND_ERROR:
		{
			MessageErrorsAll msg;
			msg.setSchedulerId(message->getSchedulerId());
			msg.dispatch();
			return;
		}

		case SearchMatch::COMMAND_LEGEND:
		{
			MessageActivateLegend msg;
			msg.setSchedulerId(message->getSchedulerId());
			msg.dispatch();
			return;
		}
		}
	}

	MessageActivateTokens m(message);
	m.isFromSearch = true;
	for (const SearchMatch& match: matches)
	{
		if (match.tokenIds.size() && match.tokenIds[0] != 0)
		{
			utility::append(m.tokenIds, match.tokenIds);
			m.searchMatches.push_back(match);
		}
	}
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageZoom* message)
{
	bool zoomIn = message->zoomIn;

	ApplicationSettings* settings = ApplicationSettings::getInstance().get();

	int fontSize = settings->getFontSize();
	int maxSize = settings->getFontSizeMax();
	int minSize = settings->getFontSizeMin();

	if ((fontSize >= maxSize && zoomIn) || (fontSize <= minSize && !zoomIn))
	{
		return;
	}

	settings->setFontSize(fontSize + (message->zoomIn ? 1 : -1));
	settings->save();

	MessageStatus(L"Font size: " + std::to_wstring(settings->getFontSize())).dispatch();
	MessageRefreshUI().dispatch();
}
