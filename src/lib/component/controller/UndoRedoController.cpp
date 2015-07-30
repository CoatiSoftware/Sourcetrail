#include "component/controller/UndoRedoController.h"

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFlushUpdates.h"

#include "component/view/UndoRedoView.h"

UndoRedoController::UndoRedoController()
	: m_lastCommand(nullptr, 0)
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
		static_cast<MessageActivateEdge*>(m_lastCommand.message.get())->name == message->name)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateEdge>(*message), (message->type == Edge::EDGE_AGGREGATION ? 0 : 1));
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

void UndoRedoController::handleMessage(MessageActivateNode* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageActivateNode*>(m_lastCommand.message.get())->name == message->name)
	{
		return;
	}

	Command command(std::make_shared<MessageActivateNode>(*message), 0);
	processCommand(command);
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

void UndoRedoController::handleMessage(MessageLoadSource* message)
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
	if (!m_lastCommand.message)
	{
		return;
	}

	if (m_lastCommand.order > 0)
	{
		replayCommands(false);
	}

	std::shared_ptr<MessageBase> msg = m_lastCommand.message;

	if (m_undo.size())
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

void UndoRedoController::handleMessage(MessageSearch* message)
{
	if (m_lastCommand.message && m_lastCommand.message->getType() == message->getType() &&
		static_cast<MessageSearch*>(m_lastCommand.message.get())->getQuery() == message->getQuery())
	{
		return;
	}

	Command command(std::make_shared<MessageSearch>(*message), 0);
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
