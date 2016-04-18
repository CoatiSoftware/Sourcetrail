#include "component/controller/UndoRedoController.h"

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/utility.h"

#include "component/view/UndoRedoView.h"
#include "data/access/StorageAccess.h"

UndoRedoController::UndoRedoController(StorageAccess* storageAccess)
	: m_activationTranslator(storageAccess)
{
	m_iterator = m_list.end();
}

UndoRedoController::~UndoRedoController()
{
}

UndoRedoView* UndoRedoController::getView()
{
	return Controller::getView<UndoRedoView>();
}

UndoRedoController::Command::Command(std::shared_ptr<MessageBase> message, Order order)
	: message(message)
	, order(order)
{
}

void UndoRedoController::handleMessage(MessageActivateEdge* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateEdge*>(lastMessage())->getFullName() == message->getFullName())
	{
		return;
	}

	Command command(
		std::make_shared<MessageActivateEdge>(*message),
		(message->isAggregation() ? Command::ORDER_ACTIVATE : Command::ORDER_ADAPT)
	);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateFile* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageActivateFile*>(lastMessage())->filePath == message->filePath)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateFile>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateLocalSymbols* message)
{
	if (sameMessageTypeAsLast(message))
	{
		static_cast<MessageActivateLocalSymbols*>(lastMessage())->symbolIds = message->symbolIds;
		return;
	}

	Command command(std::make_shared<MessageActivateLocalSymbols>(*message), Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateNodes* message)
{
	if (sameMessageTypeAsLast(message) &&
		message->nodes.size() &&
		static_cast<MessageActivateNodes*>(lastMessage())->nodes.size() == message->nodes.size() &&
		static_cast<MessageActivateNodes*>(lastMessage())->nodes[0].nameHierarchy.getQualifiedNameWithSignature() ==
		message->nodes[0].nameHierarchy.getQualifiedNameWithSignature())
	{
		return;
	}

	Command command(std::make_shared<MessageActivateNodes>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateTokenIds* message)
{
	if (sameMessageTypeAsLast(message) && message->tokenIds.size() &&
		static_cast<MessageActivateTokenIds*>(lastMessage())->tokenIds == message->tokenIds)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateTokenIds>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageChangeFileView* message)
{
	Command command(std::make_shared<MessageChangeFileView>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageDeactivateEdge* message)
{
	if (m_iterator == m_list.begin())
	{
		return;
	}

	std::list<Command>::iterator it = m_iterator;
	do
	{
		std::advance(it, -1);
	}
	while (it != m_list.begin() && it->order != Command::ORDER_ACTIVATE);

	MessageBase* m = it->message.get();
	bool keepContent = m->keepContent();

	m->setIsReplayed(false);
	m->setKeepContent(true);
	m->dispatch();

	m->setKeepContent(keepContent);
}

void UndoRedoController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	Command command(std::make_shared<MessageGraphNodeBundleSplit>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeExpand* message)
{
	Command command(std::make_shared<MessageGraphNodeExpand>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeMove* message)
{
	Command command(std::make_shared<MessageGraphNodeMove>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageLoadProject* message)
{
	clear();
}

void UndoRedoController::handleMessage(MessageRedo* message)
{
	if (m_iterator == m_list.end())
	{
		return;
	}

	std::list<Command>::iterator oldIterator = m_iterator;

	std::advance(m_iterator, 1);
	while (m_iterator->order == Command::ORDER_VIEW)
	{
		std::advance(m_iterator, 1);
	}

	getView()->setUndoButtonEnabled(true);
	if (m_iterator == m_list.end())
	{
		getView()->setRedoButtonEnabled(false);
	}

	replayCommands(oldIterator);

	MessageFlushUpdates().dispatch();
}

void UndoRedoController::handleMessage(MessageRefresh* message)
{
	if (!message->uiOnly)
	{
		return;
	}

	if (requiresActivateFallbackToken())
	{
		SearchMatch match = SearchMatch::createCommand(SearchMatch::COMMAND_ALL);
		MessageSearch msg(std::vector<SearchMatch>(1, match));
		msg.dispatch();
	}
	else
	{
		replayCommands();

		MessageFlushUpdates().dispatch();
	}
}

void UndoRedoController::handleMessage(MessageScrollCode* message)
{
	if (sameMessageTypeAsLast(message))
	{
		static_cast<MessageScrollCode*>(lastMessage())->value = message->value;
		return;
	}

	Command command(std::make_shared<MessageScrollCode>(*message), Command::ORDER_VIEW);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageSearch* message)
{
	if (sameMessageTypeAsLast(message) &&
		static_cast<MessageSearch*>(lastMessage())->getMatchesAsString() == message->getMatchesAsString())
	{
		return;
	}

	Command command(std::make_shared<MessageSearch>(*message), Command::ORDER_ACTIVATE);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowScope* message)
{
	Command command(std::make_shared<MessageShowScope>(*message), Command::ORDER_ADAPT);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageUndo* message)
{
	if (!m_list.size() || std::prev(m_iterator) == m_list.begin())
	{
		return;
	}

	while (std::prev(m_iterator)->order == Command::ORDER_VIEW)
	{
		std::advance(m_iterator, -1);
	}
	std::advance(m_iterator, -1);

	if (std::distance(m_list.begin(), m_iterator) <= 1)
	{
		getView()->setUndoButtonEnabled(false);
	}
	getView()->setRedoButtonEnabled(true);

	replayCommands();

	MessageFlushUpdates().dispatch();
}

void UndoRedoController::replayCommands()
{
	std::list<Command>::iterator startIterator = m_iterator;

	do
	{
		std::advance(startIterator, -1);
	}
	while (startIterator != m_list.begin() && startIterator->order != Command::ORDER_ACTIVATE);

	replayCommands(startIterator);
}

void UndoRedoController::replayCommands(std::list<Command>::iterator it)
{
	std::vector<std::list<Command>::iterator> viewCommands;

	std::shared_ptr<MessageBase> m;
	while (it != m_iterator)
	{
		m = it->message;
		if (it->order != Command::ORDER_VIEW)
		{
			m->setIsReplayed(true);
			m->setIsLast(it == std::prev(m_iterator));
			m->dispatch();

			viewCommands.clear();
		}
		else
		{
			viewCommands.push_back(it);
		}

		std::advance(it, 1);
	}

	std::set<std::string> messageTypes;
	std::vector<std::list<Command>::iterator> lastViewCommands;

	for (size_t i = viewCommands.size(); i > 0; i--)
	{
		it = viewCommands[i - 1];
		if (messageTypes.find(it->message->getType()) == messageTypes.end())
		{
			messageTypes.insert(it->message->getType());
			lastViewCommands.push_back(it);
		}
	}

	for (size_t i = lastViewCommands.size(); i > 0; i--)
	{
		it = lastViewCommands[i - 1];
		m = it->message;
		m->setIsReplayed(true);
		m->setIsLast(it == std::prev(m_iterator));
		m->dispatch();
	}
}

void UndoRedoController::processCommand(Command command)
{
	if (command.order == Command::ORDER_ACTIVATE && command.message->keepContent())
	{
		command.order = Command::ORDER_ADAPT;
	}

	if (!command.message->isReplayed())
	{
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
}

void UndoRedoController::clear()
{
	m_list.clear();
	m_iterator = m_list.end();

	getView()->setUndoButtonEnabled(false);
	getView()->setRedoButtonEnabled(false);
}

bool UndoRedoController::sameMessageTypeAsLast(MessageBase* message) const
{
	if (!m_list.size() || m_list.begin() == m_iterator)
	{
		return false;
	}

	return std::prev(m_iterator)->message->getType() == message->getType();
}

MessageBase* UndoRedoController::lastMessage() const
{
	return std::prev(m_iterator)->message.get();
}

bool UndoRedoController::requiresActivateFallbackToken() const
{
	std::list<Command>::iterator it = m_iterator;

	do
	{
		std::advance(it, -1);
	}
	while (it != m_list.begin() && it->order != Command::ORDER_ACTIVATE);

	return !checkCommandCausesTokenActivation(*it);
}

bool UndoRedoController::checkCommandCausesTokenActivation(const Command& command) const
{
	MessageBase* commandMessage = command.message.get();
	if (commandMessage)
	{
		std::shared_ptr<MessageActivateTokens> m;
		std::string commandMessageTypeString = commandMessage->getType();
		if (commandMessageTypeString == MessageActivateEdge::getStaticType())
		{
			m = m_activationTranslator.translateMessage(dynamic_cast<const MessageActivateEdge*>(commandMessage));
		}
		else if (commandMessageTypeString == MessageActivateFile::getStaticType())
		{
			m = m_activationTranslator.translateMessage(dynamic_cast<const MessageActivateFile*>(commandMessage));
		}
		else if (commandMessageTypeString == MessageActivateNodes::getStaticType())
		{
			MessageActivateNodes inputMessage(*dynamic_cast<MessageActivateNodes*>(commandMessage));
			inputMessage.setIsReplayed(true);
			m = m_activationTranslator.translateMessage(&inputMessage);
		}
		else if (commandMessageTypeString == MessageSearch::getStaticType())
		{
			m = m_activationTranslator.translateMessage(dynamic_cast<const MessageSearch*>(commandMessage));
		}
		return (m && m->tokenIds.size() > 0);
	}
	return false;
}
