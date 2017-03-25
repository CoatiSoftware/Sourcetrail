#include "component/controller/ActivationController.h"

#include "data/access/StorageAccess.h"
#include "settings/ApplicationSettings.h"

#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageColorSchemeTest.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageScrollToLine.h"

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
		m.tokenNames.push_back(NameHierarchy(message->getFullName()));
		m.dispatchImmediately();
	}
	else
	{
		MessageActivateTokens m(message);
		m.tokenIds.push_back(message->tokenId);
		m.isEdge = true;
		m.tokenNames.push_back(NameHierarchy(message->getFullName()));
		m.dispatchImmediately();
	}
}

void ActivationController::handleMessage(MessageActivateFile* message)
{
	Id fileId = m_storageAccess->getNodeIdForFileNode(message->filePath);

	if (fileId)
	{
		MessageActivateTokens m(message);
		m.tokenIds.push_back(fileId);
		m.tokenNames.push_back(message->filePath.str());
		m.dispatchImmediately();
	}
	else
	{
		MessageChangeFileView msg(
			message->filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			true,
			true
		);
		msg.dispatchImmediately();
	}

	if (message->line > 0)
	{
		MessageScrollToLine(message->filePath, message->line, true).dispatch();
	}
}

void ActivationController::handleMessage(MessageActivateNodes* message)
{
	MessageActivateTokens m(message);
	for (const MessageActivateNodes::ActiveNode& node : message->nodes)
	{
		Id nodeId = node.nodeId ? node.nodeId : m_storageAccess->getNodeIdForNameHierarchy(node.nameHierarchy);
		if (nodeId > 0)
		{
			m.tokenIds.push_back(nodeId);
		}
		m.tokenNames.push_back(node.nameHierarchy);
	}
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageSearch* message)
{
	const std::vector<SearchMatch>& matches = message->getMatches();

	for (const SearchMatch& match : matches)
	{
		if (match.searchType == SearchMatch::SEARCH_COMMAND)
		{
			SearchMatch::CommandType type = SearchMatch::getCommandType(match.getFullName());

			switch (type)
			{
				case SearchMatch::COMMAND_ALL:
				{
					MessageActivateAll().dispatchImmediately();
					return;
				}

				case SearchMatch::COMMAND_ERROR:
				{
					MessageShowErrors(m_storageAccess->getErrorCount()).dispatch();
					MessageFlushUpdates().dispatch();
					return;
				}

				case SearchMatch::COMMAND_COLOR_SCHEME_TEST:
				{
					MessageColorSchemeTest().dispatchImmediately();
					return;
				}
			}
		}
	}

	MessageActivateTokens m(message);
	m.tokenIds = message->getTokenIdsOfMatches();
	m.searchMatches = matches;
	m.tokenNames = m_storageAccess->getNameHierarchiesForNodeIds(m.tokenIds);
	m.isFromSearch = true;
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageActivateTokenIds* message)
{
	MessageActivateTokens m(message);
	m.tokenIds = message->tokenIds;
	m.searchMatches = m_storageAccess->getSearchMatchesForTokenIds(message->tokenIds);
	m.tokenNames = m_storageAccess->getNameHierarchiesForNodeIds(m.tokenIds);
	m.dispatchImmediately();
}

void ActivationController::handleMessage(MessageActivateSourceLocations* message)
{
	MessageActivateNodes m;
	for (Id nodeId : m_storageAccess->getNodeIdsForLocationIds(message->locationIds))
	{
		m.addNode(nodeId, m_storageAccess->getNameHierarchyForNodeId(nodeId));
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

	MessageStatus("Zoom: 100%").dispatch();
}

void ActivationController::handleMessage(MessageZoom* message)
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

