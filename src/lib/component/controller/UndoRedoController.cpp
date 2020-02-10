#include "UndoRedoController.h"

#include "MessageFlushUpdates.h"
#include "MessageSearch.h"
#include "utility.h"

#include "Application.h"
#include "Project.h"
#include "StorageAccess.h"
#include "UndoRedoView.h"

UndoRedoController::UndoRedoController(StorageAccess* storageAccess): m_storageAccess(storageAccess)
{
	m_iterator = m_list.end();
}

Id UndoRedoController::getSchedulerId() const
{
	return Controller::getTabId();
}

UndoRedoView* UndoRedoController::getView()
{
	return Controller::getView<UndoRedoView>();
}

void UndoRedoController::clear()
{
	m_list.clear();
	m_iterator = m_list.begin();

	m_historyOffset = 0;
	m_history.clear();

	updateHistoryMenu(nullptr);
	updateHistory();

	getView()->setUndoButtonEnabled(false);
	getView()->setRedoButtonEnabled(false);
}

UndoRedoController::Command::Command(std::shared_ptr<MessageBase> message, Order order, bool replayLastOnly)
	: message(message), order(order), replayLastOnly(replayLastOnly)
{
}

void UndoRedoController::handleMessage(MessageActivateErrors* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateErrors*>(lastMessage())->filter == message->filter &&
		static_cast<MessageActivateErrors*>(lastMessage())->file == message->file)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateErrors>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateFullTextSearch* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateFullTextSearch*>(lastMessage())->searchTerm == message->searchTerm &&
		static_cast<MessageActivateFullTextSearch*>(lastMessage())->caseSensitive ==
			message->caseSensitive)
	{
		return;
	}

	Command command(
		std::make_shared<MessageActivateFullTextSearch>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateLegend* message)
{
	if (sameMessageTypeAsLast(message))
	{
		return;
	}

	Command command(std::make_shared<MessageActivateLegend>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateLocalSymbols* message)
{
	if (sameMessageTypeAsLast(message))
	{
		static_cast<MessageActivateLocalSymbols*>(lastMessage())->symbolIds = message->symbolIds;
		return;
	}

	Command command(
		std::make_shared<MessageActivateLocalSymbols>(*message), Command::ORDER_VIEW, true);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateOverview* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateOverview*>(lastMessage())->acceptedNodeTypes ==
			message->acceptedNodeTypes)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateOverview>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateTokens* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateTokens*>(lastMessage())->tokenIds == message->tokenIds)
	{
		return;
	}

	Command command(
		std::make_shared<MessageActivateTokens>(*message),
		message->isEdge ? Command::ORDER_ADAPT : Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateTrail* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateTrail*>(lastMessage())->originId == message->originId &&
		static_cast<MessageActivateTrail*>(lastMessage())->targetId == message->targetId)
	{
		return;
	}

	Command command(
		std::make_shared<MessageActivateTrail>(*message),
		message->custom ? Command::ORDER_ACTIVATE : Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateTrailEdge* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateTrailEdge*>(lastMessage())->edgeIds == message->edgeIds)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateTrailEdge>(*message), Command::ORDER_ADAPT, true);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageChangeFileView* message)
{
	Command command(
		std::make_shared<MessageChangeFileView>(*message),
		message->switchesViewMode ? Command::ORDER_ADAPT : Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageCodeShowDefinition* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageCodeShowDefinition*>(lastMessage())->nodeId == message->nodeId)
	{
		return;
	}

	Command command(std::make_shared<MessageCodeShowDefinition>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageDeactivateEdge* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageDeactivateEdge*>(lastMessage())->scrollToDefinition == message->scrollToDefinition)
	{
		return;
	}

	Command command(std::make_shared<MessageDeactivateEdge>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	Command command(std::make_shared<MessageGraphNodeBundleSplit>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeExpand* message)
{
	Command command(std::make_shared<MessageGraphNodeExpand>(*message), Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeHide* message)
{
	Command command(std::make_shared<MessageGraphNodeHide>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeMove* message)
{
	Command command(std::make_shared<MessageGraphNodeMove>(*message), Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageHistoryRedo* message)
{
	if (m_iterator == m_list.end())
	{
		return;
	}

	std::list<Command>::iterator oldIterator = m_iterator;

	std::advance(m_iterator, 1);
	while (m_iterator != m_list.end() && m_iterator->order == Command::ORDER_VIEW)
	{
		std::advance(m_iterator, 1);
	}

	getView()->setUndoButtonEnabled(true);
	if (m_iterator == m_list.end())
	{
		getView()->setRedoButtonEnabled(false);
	}

	replayCommands(oldIterator);

	updateHistory();
}

void UndoRedoController::handleMessage(MessageHistoryToPosition* message)
{
	size_t index = 0;
	const size_t activeIndex = message->index + m_historyOffset;

	for (std::list<Command>::reverse_iterator rit = m_list.rbegin(); rit != m_list.rend(); rit++)
	{
		if (rit->order == Command::ORDER_ACTIVATE)
		{
			if (index == activeIndex)
			{
				std::list<Command>::iterator it = --(rit.base());
				std::list<Command>::iterator start = it;

				do
				{
					std::advance(it, 1);
				} while (it != m_list.end() && it->order != Command::ORDER_ACTIVATE);

				m_iterator = it;

				if (start != m_iterator)
				{
					replayCommands(start);
				}
				else
				{
					replayCommand(m_iterator);

					MessageFlushUpdates msg(false);
					msg.setSchedulerId(getSchedulerId());
					msg.dispatch();
				}
			}

			index++;

			if (index > activeIndex + 1)
			{
				break;
			}
		}
	}

	getView()->setUndoButtonEnabled(index > activeIndex + 1);
	getView()->setRedoButtonEnabled(
		m_iterator != m_list.end() && m_iterator->order == Command::ORDER_ACTIVATE);

	updateHistory();
}

void UndoRedoController::handleMessage(MessageHistoryUndo* message)
{
	if (!m_list.size())
	{
		return;
	}

	// return to last non view command
	std::list<Command>::iterator it = m_iterator;
	while (std::prev(it)->order == Command::ORDER_VIEW)
	{
		std::advance(it, -1);
	}
	std::advance(it, -1);

	// disable undo button if there is no non view command till the first command
	std::list<Command>::iterator it2 = std::prev(it);
	while (it2->order == Command::ORDER_VIEW)
	{
		std::advance(it2, -1);
	}
	if (it2 == m_list.begin())
	{
		getView()->setUndoButtonEnabled(false);
	}

	// abort if first command is reached
	if (it == m_list.begin())
	{
		return;
	}

	getView()->setRedoButtonEnabled(true);

	m_iterator = it;

	replayCommands();

	updateHistory();
}

void UndoRedoController::handleMessage(MessageIndexingFinished* message)
{
	std::list<Command> newList;

	for (const Command& command: m_list)
	{
		if (command.order == Command::ORDER_ACTIVATE)
		{
			MessageActivateTokens* msg = dynamic_cast<MessageActivateTokens*>(command.message.get());
			if (msg)
			{
				if (msg->isAggregation)
				{
					continue;
				}
				msg->isFromSearch = false;
			}

			newList.insert(newList.end(), command);
		}
	}

	m_list = newList;
	m_iterator = m_list.end();
}

void UndoRedoController::handleMessage(MessageRefreshUIState* message)
{
	std::list<Command>::iterator startIterator = m_iterator;
	do
	{
		std::advance(startIterator, -1);
	} while (startIterator != m_list.begin() && startIterator->order != Command::ORDER_ACTIVATE);

	if (startIterator == m_list.begin() ||
		(message->isAfterIndexing &&
		 dynamic_cast<MessageActivateErrors*>(startIterator->message.get()) &&
		 m_storageAccess->getErrorCount().total == 0))
	{
		MessageActivateOverview msg;
		msg.setSchedulerId(getSchedulerId());
		msg.dispatch();
	}
	else
	{
		replayCommands();
	}
}

void UndoRedoController::handleMessage(MessageScrollCode* message)
{
	if (sameMessageTypeAsLast(message))
	{
		static_cast<MessageScrollCode*>(lastMessage())->value = message->value;
		return;
	}

	Command command(std::make_shared<MessageScrollCode>(*message), Command::ORDER_VIEW, true);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageScrollGraph* message)
{
	if (sameMessageTypeAsLast(message))
	{
		static_cast<MessageScrollGraph*>(lastMessage())->xValue = message->xValue;
		static_cast<MessageScrollGraph*>(lastMessage())->yValue = message->yValue;
		return;
	}

	Command command(std::make_shared<MessageScrollGraph>(*message), Command::ORDER_VIEW, true);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowError* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageShowError*>(lastMessage())->errorId == message->errorId)
	{
		return;
	}

	Command command(std::make_shared<MessageShowError>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowReference* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageShowReference*>(lastMessage())->refIndex == message->refIndex)
	{
		return;
	}

	Command command(std::make_shared<MessageShowReference>(*message), Command::ORDER_VIEW, true);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowScope* message)
{
	Command command(std::make_shared<MessageShowScope>(*message), Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::replayCommands()
{
	std::list<Command>::iterator startIterator = m_iterator;

	do
	{
		std::advance(startIterator, -1);
	} while (startIterator != m_list.begin() && startIterator->order != Command::ORDER_ACTIVATE);

	replayCommands(startIterator);
}

void UndoRedoController::replayCommands(std::list<Command>::iterator it)
{
	std::map<std::string, std::list<Command>::iterator> lastOfType;
	std::list<Command>::iterator at = it;
	while (at != m_iterator)
	{
		if (at->replayLastOnly)
		{
			lastOfType[at->message->getType()] = at;
		}

		std::advance(at, 1);
	}

	bool keepsContent = true;

	while (it != m_iterator)
	{
		if (!it->replayLastOnly || lastOfType[it->message->getType()] == it)
		{
			replayCommand(it);

			if (!it->message->keepContent())
			{
				keepsContent = false;
			}
		}

		std::advance(it, 1);
	}

	MessageFlushUpdates msg(keepsContent);
	msg.setSchedulerId(getSchedulerId());
	msg.dispatch();
}

void UndoRedoController::replayCommand(std::list<Command>::iterator it)
{
	std::shared_ptr<MessageBase> m = it->message;

	if (m->getType() == MessageActivateTokens::getStaticType())
	{
		MessageActivateTokens* msg = dynamic_cast<MessageActivateTokens*>(m.get());

		if (!msg->isEdge && !msg->isAggregation)
		{
			std::vector<SearchMatch> matches = msg->getSearchMatches();
			msg->searchMatches.clear();
			msg->tokenIds.clear();

			for (SearchMatch match: matches)
			{
				// TODO: replace duplicate main definition fix with better solution
				if (match.nodeType.getType() != NodeType::NODE_FUNCTION ||
					!match.tokenNames.size() || match.tokenNames[0].getRawName() != L"main")
				{
					match.tokenIds = m_storageAccess->getNodeIdsForNameHierarchies(match.tokenNames);
				}

				if (!match.tokenIds.size())
				{
					match.nodeType = NodeType::NODE_SYMBOL;
				}

				utility::append(msg->tokenIds, match.tokenIds);
				msg->searchMatches.push_back(match);
			}
		}
	}
	else if (m->getType() == MessageActivateErrors::getStaticType())
	{
		std::shared_ptr<const Project> currentProject =
			Application::getInstance()->getCurrentProject();
		if (currentProject && currentProject->isIndexing())
		{
			Application::getInstance()->handleDialog(L"Errors cannot be activated while indexing.");

			ErrorFilter filter;
			filter.error = false;
			filter.fatal = false;
			filter.unindexedError = false;
			filter.unindexedFatal = false;

			MessageActivateErrors msg(filter);
			msg.setSchedulerId(getSchedulerId());
			msg.setIsReplayed(true);
			msg.setIsLast(it == std::prev(m_iterator));
			msg.dispatch();

			return;
		}
	}

	m->setIsReplayed(true);
	m->setIsLast(it == std::prev(m_iterator));

	m->dispatch();

	m->setIsReplayed(false);
	m->setIsLast(false);
}

void UndoRedoController::processCommand(Command command)
{
	if (!command.message->isReplayed())
	{
		if (command.order != Command::ORDER_ACTIVATE && m_iterator == m_list.begin())
		{
			return;
		}

		if (command.order == Command::ORDER_ACTIVATE && command.message->keepContent())
		{
			command.order = Command::ORDER_ADAPT;
		}

		if (command.order == Command::ORDER_ACTIVATE)
		{
			m_iterator = m_list.erase(m_iterator, m_list.end());
		}
		else if (command.order == Command::ORDER_ADAPT)
		{
			std::list<Command>::iterator end = m_iterator;
			while (end != m_list.end())
			{
				if (end->order == Command::ORDER_ACTIVATE)
				{
					break;
				}
				std::advance(end, 1);
			}

			m_iterator = m_list.erase(m_iterator, end);
		}

		m_list.insert(m_iterator, command);

		if (command.order != Command::ORDER_VIEW)
		{
			if (m_list.begin() != std::prev(m_iterator))
			{
				getView()->setUndoButtonEnabled(true);
			}

			if (m_list.end() == m_iterator)
			{
				getView()->setRedoButtonEnabled(false);
			}
		}
	}

	if (command.order == Command::ORDER_ACTIVATE)
	{
		updateHistoryMenu(command.message);
		updateHistory();
	}
}

bool UndoRedoController::sameMessageTypeAsLast(MessageBase* message) const
{
	if (message->isReplayed())
	{
		return false;
	}

	if (!m_list.size() || m_list.begin() == m_iterator)
	{
		return false;
	}

	return lastMessage()->getType() == message->getType();
}

MessageBase* UndoRedoController::lastMessage() const
{
	return std::prev(m_iterator)->message.get();
}

void UndoRedoController::updateHistoryMenu(std::shared_ptr<MessageBase> message)
{
	Application::getInstance()->updateHistoryMenu(message);
}

void UndoRedoController::updateHistory()
{
	const size_t historyListSize = 50;
	std::vector<SearchMatch> historyListMatches;

	size_t index = 0;
	int currentIndex = -1;
	m_historyOffset = 0;

	for (std::list<Command>::const_reverse_iterator it = m_list.rbegin(); it != m_list.rend(); it++)
	{
		if (m_iterator == it.base())
		{
			currentIndex = index;
		}

		if (it->order == Command::ORDER_ACTIVATE &&
			dynamic_cast<MessageActivateBase*>(it->message.get()))
		{
			index++;

			std::vector<SearchMatch> matches =
				dynamic_cast<MessageActivateBase*>(it->message.get())->getSearchMatches();
			if (!matches.size() || matches[0].text.empty())
			{
				continue;
			}

			historyListMatches.push_back(matches[0]);

			if (historyListMatches.size() > historyListSize)
			{
				historyListMatches.erase(historyListMatches.begin());
				m_historyOffset++;
			}

			if (historyListMatches.size() == historyListSize && currentIndex != -1 &&
				currentIndex - m_historyOffset != historyListSize - 1)
			{
				break;
			}
		}
	}

	getView()->updateHistory(historyListMatches, currentIndex - m_historyOffset);
}

void UndoRedoController::dump() const
{
	std::cout << "\nUndo Redo Stack:\n----------\n";

	std::list<Command>::const_iterator it = m_list.begin();
	while (it != m_list.end())
	{
		switch (it->order)
		{
		case Command::ORDER_VIEW:
			std::cout << "\t";
		case Command::ORDER_ADAPT:
			std::cout << "\t";
		case Command::ORDER_ACTIVATE:
			break;
		}

		std::cout << it->message->getType();

		if (it == m_iterator)
		{
			std::cout << " <-";
		}
		std::cout << std::endl;

		std::advance(it, 1);
	}

	if (m_list.end() == m_iterator)
	{
		std::cout << "<-" << std::endl;
	}
	std::cout << "----------" << std::endl;
}
