#include "component/controller/UndoRedoController.h"

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFlushUpdates.h"

#include "component/view/UndoRedoView.h"
#include "data/access/StorageAccess.h"

UndoRedoController::UndoRedoController(StorageAccess* storageAccess)
	: m_activationTranslator(storageAccess)
	, m_storageAccess(storageAccess)
	, m_lastCommand(nullptr, 0)
{
}

UndoRedoController::~UndoRedoController()
{
}

UndoRedoView* UndoRedoController::getView()
{
	return Controller::getView<UndoRedoView>();
}

UndoRedoController::Command::Command(std::shared_ptr<MessageBase> message, size_t order)
	: message(message)
	, order(order)
{
}

void UndoRedoController::handleMessage(MessageActivateEdge* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageActivateEdge*>(m_lastCommand.message.get())->getFullName() == message->getFullName())
	{
		return;
	}

	Command command(std::make_shared<MessageActivateEdge>(*message), (message->isAggregation() ? 0 : 1));
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateFile* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageActivateFile*>(m_lastCommand.message.get())->filePath == message->filePath)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateFile>(*message), 0);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateNodes* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() && message->nodes.size() &&
		static_cast<MessageActivateNodes*>(m_lastCommand.message.get())->nodes.size() == message->nodes.size() &&
		static_cast<MessageActivateNodes*>(m_lastCommand.message.get())->nodes[0].nameHierarchy.getFullName() ==
			message->nodes[0].nameHierarchy.getFullName())
	{
		return;
	}

	Command command(std::make_shared<MessageActivateNodes>(*message), 0);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageActivateTokenIds* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() && message->tokenIds.size() &&
		static_cast<MessageActivateTokenIds*>(m_lastCommand.message.get())->tokenIds == message->tokenIds)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateTokenIds>(*message), 0);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageDeactivateEdge* message)
{
	MessageBase* m = nullptr;

	if (m_lastCommand.message && m_lastCommand.order == 0)
	{
		m = m_lastCommand.message.get();
	}
	else if (m_undo.size())
	{
		int i = m_undo.size() - 1;
		while (i >= 0 && m_undo[i].order > 0)
		{
			i--;
		}
		m = m_undo[i].message.get();
	}

	if (m)
	{
		bool keepContent = m->keepContent();
		m->undoRedoType = MessageBase::UNDOTYPE_NORMAL;
		m->setKeepContent(true);
		m->dispatch();
		m->setKeepContent(keepContent);
	}
}

void UndoRedoController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	Command command(std::make_shared<MessageGraphNodeBundleSplit>(*message), 1);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeExpand* message)
{
	Command command(std::make_shared<MessageGraphNodeExpand>(*message), 1);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageGraphNodeMove* message)
{
	Command command(std::make_shared<MessageGraphNodeMove>(*message), 1);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageLoadProject* message)
{
	clear();
}

void UndoRedoController::handleMessage(MessageRedo* message)
{
	if (!m_redo.empty())
	{
		std::shared_ptr<MessageBase> m = m_redo.back().message;
		m_redo.pop_back();
		m->undoRedoType = MessageBase::UNDOTYPE_REDO;
		m->dispatch();

		MessageFlushUpdates().dispatch();
	}
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
		if (m_lastCommand.order > 0)
		{
			replayCommands(false);
		}

		std::shared_ptr<MessageBase> msg = m_lastCommand.message;

		if (m_undo.size() > 0)
		{
			m_lastCommand = m_undo.back();
			m_undo.pop_back();
		}
		else
		{
			m_lastCommand.message.reset();
		}

		msg->undoRedoType = MessageBase::UNDOTYPE_REDO;
		msg->dispatch();

		MessageFlushUpdates().dispatch();
	}
}

void UndoRedoController::handleMessage(MessageSearch* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageSearch*>(m_lastCommand.message.get())->getMatchesAsString() == message->getMatchesAsString())
	{
		return;
	}

	Command command(std::make_shared<MessageSearch>(*message), 0);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowErrors* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType())
	{
		return;
	}

	Command command(std::make_shared<MessageShowErrors>(*message), 0);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowFile* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageShowFile*>(m_lastCommand.message.get())->filePath == message->filePath)
	{
		return;
	}

	Command command(std::make_shared<MessageShowFile>(*message), 1);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageShowScope* message)
{
	Command command(std::make_shared<MessageShowScope>(*message), 1);
	processCommand(command);
}

void UndoRedoController::handleMessage(MessageUndo* message)
{
	replayCommands(true);

	MessageFlushUpdates().dispatch();
}

void UndoRedoController::replayCommands(bool removeLast)
{
	if (!m_undo.empty())
	{
		int i = m_undo.size() - 1;
		while (i >= 1 && m_undo[i].order > 0)
		{
			i--;
		}

		std::shared_ptr<MessageBase> m;
		while (i < int(m_undo.size() - 1))
		{
			m = m_undo[i].message;
			m->undoRedoType = MessageBase::UNDOTYPE_IGNORE;
			m->dispatch();
			i++;
		}

		m = m_undo.back().message;

		if (removeLast)
		{
			m_undo.pop_back();
			m->undoRedoType = MessageBase::UNDOTYPE_UNDO;
		}
		else
		{
			m->undoRedoType = MessageBase::UNDOTYPE_IGNORE;
		}

		m->dispatch();
	}
}

void UndoRedoController::processCommand(const Command& command)
{
	switch (command.message->undoRedoType)
	{
	case MessageBase::UNDOTYPE_NORMAL:
		processNormalCommand(command);
		break;
	case MessageBase::UNDOTYPE_REDO:
		processRedoCommand(command);
		break;
	case MessageBase::UNDOTYPE_UNDO:
		processUndoCommand(command);
		break;
	case MessageBase::UNDOTYPE_IGNORE:
		break;
	}
}

void UndoRedoController::processNormalCommand(const Command& command)
{
	if (m_lastCommand.message)
	{
		m_undo.push_back(m_lastCommand);
		getView()->setUndoButtonEnabled(true);
	}

	m_lastCommand = command;

	m_redo.clear();
	getView()->setRedoButtonEnabled(false);
}

void UndoRedoController::processRedoCommand(const Command& command)
{
	if (m_lastCommand.message)
	{
		m_undo.push_back(m_lastCommand);
		getView()->setUndoButtonEnabled(true);
	}

	m_lastCommand = command;

	if (m_redo.empty())
	{
		getView()->setRedoButtonEnabled(false);
	}
}

void UndoRedoController::processUndoCommand(const Command& command)
{
	if (m_lastCommand.message)
	{
		m_redo.push_back(m_lastCommand);
		getView()->setRedoButtonEnabled(true);
	}

	m_lastCommand = command;

	if (m_undo.empty())
	{
		getView()->setUndoButtonEnabled(false);
	}
}

void UndoRedoController::clear()
{
	m_lastCommand = Command(nullptr, 0);

	m_undo.clear();
	m_redo.clear();

	getView()->setUndoButtonEnabled(false);
	getView()->setRedoButtonEnabled(false);
}

bool UndoRedoController::requiresActivateFallbackToken() const
{
	bool activateFallbackToken = true;
	if (m_lastCommand.message)
	{
		if (m_lastCommand.order == 0)
		{
			activateFallbackToken = !checkCommandCausesTokenActivation(m_lastCommand);
		}
		else
		{
			for (int i = m_undo.size() - 1; i >= 0; i--)
			{
				if (m_undo[i].order == 0)
				{
					activateFallbackToken = !checkCommandCausesTokenActivation(m_undo[i]);
					break;
				}
			}
		}
	}
	return activateFallbackToken;
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
			inputMessage.undoRedoType = MessageBase::UNDOTYPE_REDO;
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
