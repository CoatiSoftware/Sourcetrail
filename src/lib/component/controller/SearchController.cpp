#include "component/controller/SearchController.h"

#include "component/view/SearchView.h"
#include "data/access/GraphAccess.h"

SearchController::SearchController(GraphAccess* graphAccess)
	: m_graphAccess(graphAccess)
{
}

SearchController::~SearchController()
{
}

void SearchController::search(const std::string& s)
{
	LOG_INFO("searching string: \"" + s + "\"");
	Id nodeId = m_graphAccess->getIdForNodeWithName(s);
	if (nodeId > 0)
	{
		LOG_INFO("Node with name \"" + s + "\" found.");
		MessageActivateToken message(nodeId);
		message.dispatch();
	}
	else
	{
		LOG_INFO("Node with name \"" + s + "\" not found.");
	}
}

void SearchController::handleMessage(MessageActivateToken* message)
{
	getView()->setText(m_graphAccess->getNameForNodeWithId(message->tokenId));
}

void SearchController::handleMessage(MessageFind* message)
{
	getView()->setFocus();
}

void SearchController::handleMessage(MessageFinishedParsing* message)
{
	getView()->setAutocompletionList(m_graphAccess->getNamesForNodesWithNamePrefix(""));
}

void SearchController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
}

SearchView* SearchController::getView()
{
	return Controller::getView<SearchView>();
}
