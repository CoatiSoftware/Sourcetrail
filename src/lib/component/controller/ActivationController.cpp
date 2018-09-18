#include "ActivationController.h"

#include "StorageAccess.h"
#include "ApplicationSettings.h"

#include "MessageActivateLegend.h"
#include "MessageErrorsAll.h"
#include "MessageActivateAll.h"
#include "MessageActivateTokens.h"
#include "MessageChangeFileView.h"
#include "MessageFlushUpdates.h"
#include "MessageRefresh.h"
#include "MessageStatus.h"
#include "MessageScrollToLine.h"
#include "utility.h"

ActivationController::ActivationController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

ActivationController::~ActivationController()
{
}

void ActivationController::clear()
{
}

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
		messageActivateTokens.searchMatches = m_storageAccess->getSearchMatchesForTokenIds({ fileId });
		messageActivateTokens.dispatchImmediately();
	}
	else
	{
		MessageChangeFileView msg(
			message->filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			MessageChangeFileView::VIEW_CURRENT,
			true,
			true
		);
		msg.dispatchImmediately();
	}

	if (message->line > 0)
	{
		MessageScrollToLine(message->filePath, message->line).dispatch();
	}
}

void ActivationController::handleMessage(MessageActivateNodes* message)
{
	MessageActivateTokens m(message);
	for (const MessageActivateNodes::ActiveNode& node : message->nodes)
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
	MessageActivateNodes m;
	for (Id nodeId : m_storageAccess->getNodeIdsForLocationIds(message->locationIds))
	{
		m.addNode(nodeId);
	}
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageResetZoom* message)
{
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	int fontSizeStd = settings->getFontSizeStd();

	if (settings->getFontSize() != fontSizeStd)
	{
		settings->setFontSize(fontSizeStd);
		settings->save();

		MessageRefresh().refreshUiOnly().dispatch();
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
				MessageActivateAll(message->acceptedNodeTypes).dispatchImmediately();
				return;
			}

			case SearchMatch::COMMAND_ERROR:
			{
				MessageErrorsAll().dispatch();
				return;
			}

			case SearchMatch::COMMAND_LEGEND:
			{
				MessageActivateLegend().dispatch();
				return;
			}
		}
	}

	MessageActivateTokens m(message);
	m.isFromSearch = true;
	for (const SearchMatch& match : matches)
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

	if ((fontSize >= maxSize && zoomIn)
		|| (fontSize <= minSize && !zoomIn))
	{
		return;
	}

	settings->setFontSize(fontSize + (message->zoomIn ? 1 : -1));
	settings->save();

	MessageStatus(L"Font size: " + std::to_wstring(settings->getFontSize())).dispatch();
	MessageRefresh().refreshUiOnly().dispatch();
}

